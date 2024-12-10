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

    HE::Bool AssetManagerModule::_VStart()
    {
        // 利用するアセット数を設定
        this->_ReservePool(1024);

        return TRUE;
    }

    void AssetManagerModule::Unload(const Core::Common::Handle& in_rHandle)
    {
        if (in_rHandle.Null()) return;

        if (this->Empty()) return;

        AssetDataBase* p = this->_Ref(in_rHandle);
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
            auto assetList = this->GetUserDataList();
            for (auto it = assetList->begin(); it != assetList->end(); ++it)
            {
                this->Unload(it->first);
            }
        }

        this->_ReleasePool();

        return TRUE;
    }

}  // namespace AssetManager
