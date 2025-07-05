#include "LocalizationModule.h"

namespace Localization
{
    LocalizationModule::LocalizationModule() : ModuleBase(ModuleName())
    {
        this->_AppendDependenceModule<AssetManager::AssetManagerModule>();
    }

    HE::Bool LocalizationModule::_VStart()
    {
        return TRUE;
    }

    HE::Bool LocalizationModule::_VRelease()
    {
        // ロードしたアセットを全て破棄
        for (auto it = this->_locateDataMap.Begin(); it != this->_locateDataMap.End(); ++it)
        {
            this->UnloadTextAll(it->_key);
        }

        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        pLocalModule->Unload(this->_sysAssetDataHandle);

        return TRUE;
    }

    HE::Bool LocalizationModule::LoadSystemFile(const Core::Common::StringBase& in_szrFilePath)
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        // tomlファイルをロード
        auto szAssetName = HE_STR_TEXT("LSystem");

        this->_sysAssetDataHandle =
            pLocalModule->Load<SystemAssetData>(szAssetName, in_szrFilePath.Str());
        HE_ASSERT(this->_sysAssetDataHandle.Null() == FALSE);
        if (this->_sysAssetDataHandle.Null()) return FALSE;

        return TRUE;
    }

    HE::Bool LocalizationModule::UnloadSystemFile()
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        pLocalModule->Unload(this->_sysAssetDataHandle);
        return TRUE;
    }

    HE::Bool LocalizationModule::LoadTextAll(const Core::Common::StringBase& in_szLocateName)
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        SystemAssetData& a = pLocalModule->GetAsset<SystemAssetData>(this->_sysAssetDataHandle);
        const SystemAssetData::LocateMap& map = a.FindLocate(in_szLocateName.Str());
        HE_ASSERT(map.Empty() == FALSE);

        LocateTextMap textMap;
        Core::Common::FixedString128 szTmp;
        for (auto it = map.Begin(); it != map.End(); ++it)
        {
            szTmp = in_szLocateName.Str();
            szTmp.Concatenate(HE_STR_TEXT("/"), it->_key.Str());

            auto h =
                pLocalModule->Load<LocateAssetData>(szTmp.Str(), it->_data._textFilePath.Str());
            textMap.Add(it->_key, h);
        }
        HE_ASSERT(0 < textMap.Size());

        this->_locateDataMap.Add(in_szLocateName.Str(), textMap);

        return TRUE;
    }

    HE::Bool LocalizationModule::UnloadTextAll(const Core::Common::StringBase& in_szLocateName)
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        auto pMap = &this->_locateDataMap[in_szLocateName.Str()];
        for (auto it = pMap->Begin(); it != pMap->End(); ++it)
        {
            pLocalModule->Unload(it->_data);
        }
        pMap->Clear();

        return TRUE;
    }

    const HE::Char* LocalizationModule::Text(const HE::Char* in_szLocateName,
                                             const HE::Char* in_szGroupName,
                                             const HE::Char* in_szKey)
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        auto locateIter = this->_locateDataMap.FindKey(in_szLocateName);
        HE_ASSERT(locateIter.IsValid());

        auto groupIter = locateIter->_data.FindKey(in_szGroupName);
        HE_ASSERT(groupIter.IsValid());

        LocateAssetData& rData = pLocalModule->GetAsset<LocateAssetData>(groupIter->_data);

        // テキストを取得
        Core::Common::g_szTempFixedString1024 = in_szKey;
        HE_ASSERT(Core::Common::g_szTempFixedString1024.Size() <= 256);

        // 作業用の変数
        static HE::UTF8 szKeyByTempBuff[256] = {0};

        Core::Common::g_szTempFixedString1024.OutputUTF8(szKeyByTempBuff,
                                                         HE_ARRAY_NUM(szKeyByTempBuff));
        return rData.GetText(szKeyByTempBuff).Str();
    }

    HE::Bool SystemAssetData::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        const HE::Bool bRet = AssetManager::AssetDataToml::_VLoad(in_rFileSystem);
        HE_ASSERT(bRet);

        // データがあるカレントディレクトリ名を取得
        {
            auto spCurrendDirNode =
                this->VGetNodeByName({HE_STR_U8_TEXT("config"), HE_STR_U8_TEXT("current_dir")});
            spCurrendDirNode->VOutputString(&Core::Common::g_szTempFixedString128);
        }

        Core::File::Path dataRootPath(Core::Common::g_szTempFixedString128.Str());

        Core::Common::FixedString128 locateStr;
        HE::Sint32 sLocateLevel = 0;

        // TODO: 作業のローカル変数だが、スタックだとメモリを沢山使うのでstaticした
        // ヒープに切り替えた方がいいかも
        static Core::Common::FixedMap<Core::Common::FixedString128, LocateData, 32> mData;
        mData.Clear();

        while (1)
        {
            auto spLocateNode = this->VGetNodeByLevel({HE_STR_U8_TEXT("locate")}, sLocateLevel);
            if (spLocateNode->IsNone()) break;

            ++sLocateLevel;

            // 言語名
            locateStr = spLocateNode->Name();
            locateStr.ToUpper();

            HE::Sint32 sGroupLevel = 0;
            while (1)
            {
                auto spGroupNode =
                    this->VGetNodeByLevel({HE_STR_U8_TEXT("locate"), spLocateNode->Name()},
                                          sGroupLevel);
                if (spGroupNode->IsNone()) break;

                ++sGroupLevel;

                // ファイルパス連結する
                Core::File::Path path(dataRootPath);
                path += Core::File::Path(locateStr.Str());

                spGroupNode->VOutputString(&Core::Common::g_szTempFixedString512,
                                           HE_STR_U8_TEXT("json"));
                path += Core::File::Path(Core::Common::g_szTempFixedString512.Str());

                LocateData _data(path);
                HE_LOG_LINE(HE_STR_TEXT("%s"), _data._textFilePath.Str());

                mData.Add(spGroupNode->Name(), _data);
            }

            this->_locateDataMap.Add(locateStr, mData);
            mData.Clear();
        }

        return TRUE;
    }

    const SystemAssetData::LocateMap& SystemAssetData::FindLocate(const HE::Char* in_szLocateName)
    {
        HE_ASSERT(in_szLocateName);

        Core::Common::FixedString128 key(in_szLocateName);
        auto it = this->_locateDataMap.FindKey(key);
        return it->_data;
    }

    const HE::Uint32 LocateAssetData::GetTextSize(const HE::UTF8* in_szKey)
    {
        auto spNode = this->VGetNodeByName({in_szKey, HE_STR_U8_TEXT("size")});
        return spNode->VGetUInt32();
    }

    const HE::Uint32 LocateAssetData::GetTextColor(const HE::UTF8* in_szKey)
    {
        auto spNode = this->VGetNodeByName({in_szKey, HE_STR_U8_TEXT("color")});
        return spNode->VGetUInt32();
    }

    const Core::Common::StringBase& LocateAssetData::GetText(const HE::UTF8* in_szKey)
    {
        // キャッシュしているテキストがなければデータからテキストを取る
        if (this->_textBuffMap.Contains(in_szKey) == FALSE)
        {
            auto spNode = this->VGetNodeByName(
                {in_szKey, HE_STR_U8_TEXT("items"), HE_STR_U8_TEXT("0"), HE_STR_U8_TEXT("text")});

            spNode->VOutputString(&Core::Common::g_szTempFixedString1024);
            this->_textBuffMap.Add(in_szKey, Core::Common::g_szTempFixedString1024);
        }

        return this->_textBuffMap[in_szKey];
    }
}  // namespace Localization
