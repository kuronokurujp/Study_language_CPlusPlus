#pragma once

#include "AssetManager/AssetDataBase.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

namespace AssetManager
{
    /// <summary>
    /// xmlファイルを扱うアセットデータ
    /// </summary>
    class AssetDataXml : public AssetDataBase, public InterfaceTreeData
    {
        HE_CLASS_COPY_NG(AssetDataXml);
        HE_CLASS_MOVE_NG(AssetDataXml);

    public:
        AssetDataXml() : AssetDataBase() {}
        virtual ~AssetDataXml() = default;

        virtual NodeSharedPtr VGetNodeByName(
            const std::initializer_list<const HE::UTF8*>& in_aName) override;

        virtual NodeSharedPtr VGetNodeByName(
            AbstractTreeNode& in_rLocateNode,
            const std::initializer_list<const HE::UTF8*>& in_aName) override;

        virtual NodeSharedPtr VGetNodeByLevel(
            const std::initializer_list<const HE::UTF8*>& in_aName,
            const HE::Sint32 in_uLevel) override;

        virtual NodeSharedPtr VGetNodeByLevel(AbstractTreeNode& in_rCurrentNode,
                                              const HE::Sint32 in_uLevel) override;

    protected:
        virtual HE::Bool _VLoad(Platform::FileInterface&) override;
        virtual void _VUnload() override;

    protected:
        Core::Common::Handle _fileHandle;
        void* _pDoc = NULL;
    };

}  // namespace AssetManager
