#pragma once

#include "AssetManager/AssetDataBase.h"
#include "Engine/Common/CustomMap.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

namespace AssetManager
{
    /// <summary>
    /// tomlアセットデータ
    /// </summary>
    class AssetDataToml : public AssetDataBase, public InterfaceTreeData
    {
        HE_CLASS_COPY_NG(AssetDataToml);
        HE_CLASS_MOVE_NG(AssetDataToml);

    public:
        AssetDataToml() : AssetDataBase() {}

        virtual NodeSharedPtr VGetNodeByName(
            const std::initializer_list<const HE::UTF8*>& in_aName);

        virtual NodeSharedPtr VGetNodeByName(
            AbstractTreeNode& in_rCurrentNode,
            const std::initializer_list<const HE::UTF8*>& in_aName) override;

        virtual NodeSharedPtr VGetNodeByLevel(
            const std::initializer_list<const HE::UTF8*>& in_aName,
            const HE::Sint32 in_uLevel) override;

        virtual NodeSharedPtr VGetNodeByLevel(AbstractTreeNode& in_rCurrentNode,
                                              const HE::Sint32 in_uLevel) override;

    protected:
        virtual HE::Bool _VLoad(Platform::FileInterface&) override;
        virtual void _VUnload() override;

    private:
        void* _pResult   = NULL;
        HE::Char* _pText = NULL;
    };

}  // namespace AssetManager
