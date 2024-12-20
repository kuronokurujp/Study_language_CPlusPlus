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
    void AssetManagerModule::SetCurrentDir(const HE::Char* in_szMountDir)
    {
        // TODO: プラットフォームにファイル参照のカレントディレクトリを設定
        auto pPlatform = this->GetDependenceModule<Platform::PlatformModule>();
        pPlatform->VFile()->SetCurrentDir(in_szMountDir);
    }

    HE::Bool AssetManagerModule::_VStart()
    {
        // 利用するアセット数を設定
        this->_poolAssetDataManager.ReservePool(1024);

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

    HE::Bool AssetManagerModule::_Load(AssetDataBase* out)
    {
        auto pPlatformModule = this->GetDependenceModule<Platform::PlatformModule>();
        HE_ASSERT(pPlatformModule);

        auto spFileSystem = pPlatformModule->VFile();
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
