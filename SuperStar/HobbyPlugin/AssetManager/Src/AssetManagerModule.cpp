#include "AssetManagerModule.h"

#include "AssetManager/AssetDataBase.h"
#include "Engine/Platform/PlatformModule.h"

namespace AssetManager
{
    AssetManagerModule::AssetManagerModule() : ModuleBase(ModuleName())
    {
        // 依存モジュール
        this->_AppendDependenceModule<Platform::PlatformModule>();
    }

    /// <summary>
    /// アセット格納ディレクトリ設定
    /// </summary>
    void AssetManagerModule::SetCurrentDir(const Core::File::Path& in_rMountDirPath)
    {
        // TODO: プラットフォームにファイル参照のカレントディレクトリを設定
        auto pPlatform = this->GetDependenceModule<Platform::PlatformModule>();
        pPlatform->File()->SetCurrentDir(in_rMountDirPath);
    }

    HE::Bool AssetManagerModule::_VStart()
    {
        // 利用するアセット数を設定
        this->_poolAssetDataManager.POOL_RESERVE_POOL(1024);

        return TRUE;
    }

    void AssetManagerModule::Unload(const Core::Common::Handle& in_rHandle)
    {
        if (in_rHandle.Null()) return;

        if (this->_poolAssetDataManager.Empty()) return;

        AssetDataBase* p = this->_poolAssetDataManager.Ref(in_rHandle);
        if (p == NULL) return;

        p->_VUnload();
    }

    const Core::Common::Handle AssetManagerModule::GetAssetHandle(
        const Core::File::Path& in_rFilePath)
    {
        auto pPlatformModule = this->GetDependenceModule<Platform::PlatformModule>();
        HE_ASSERT(pPlatformModule);

        // TODO: 一つ一つチェックするので処理速度は遅い
        auto mAsset = this->_poolAssetDataManager.GetUserDataList();
        for (auto itr = mAsset->begin(); itr != mAsset->end(); ++itr)
        {
            if (HE_STR_CMP(itr->second->_path.Str(), in_rFilePath.Str()) == 0)
            {
                return itr->first;
            }
        }

        return NullHandle;
    }

    HE::Bool AssetManagerModule::_Load(AssetDataBase* out)
    {
        auto pPlatformModule = this->GetDependenceModule<Platform::PlatformModule>();
        HE_ASSERT(pPlatformModule);

        auto spFileSystem = pPlatformModule->File();
        if (out->_VLoad(*spFileSystem) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }

    HE::Bool AssetManagerModule::_VRelease()
    {
        // アセットハンドルがあれば解放する
        {
            auto assetList = this->_poolAssetDataManager.GetUserDataList();
            HE_ASSERT(assetList);

            {
                for (auto it = assetList->begin(); it != assetList->end(); ++it)
                {
                    this->Unload(it->first);
                }
            }
        }

        this->_poolAssetDataManager.ReleasePool();

        return TRUE;
    }

}  // namespace AssetManager
