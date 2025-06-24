#pragma once

#include "AssetManager/AssetDataBase.h"
#include "Engine/Memory/Memory.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

namespace AssetManager
{
    /// <summary>
    /// jsonアセットデータ
    /// </summary>
    class AssetDataJson : public AssetDataBase, public InterfaceTreeData
    {
        HE_CLASS_COPY_NG(AssetDataJson);
        HE_CLASS_MOVE_NG(AssetDataJson);

    public:
        AssetDataJson() : AssetDataBase() {}
        virtual ~AssetDataJson() = default;

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

        /// <summary>
        /// 指定したトークンが存在するか
        /// </summary>
        HE::Bool IsToken(const std::initializer_list<const HE::UTF8*>&);

    protected:
        virtual HE::Bool _VLoad(Platform::FileInterface&) override;
        virtual void _VUnload() override;

    protected:
        Core::Common::Handle _fileHandle;

        void* _pJson   = NULL;
        void* _pParser = NULL;
    };
}  // namespace AssetManager
