#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/File/Path.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

// 前方宣言
namespace Platform
{
    class FileInterface;
}

namespace AssetManager
{
    /// <summary>
    /// アセットデータの基本
    /// </summary>
    class AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataBase);
        HE_CLASS_MOVE_NG(AssetDataBase);

        friend class AssetManagerModule;

    public:
        AssetDataBase()          = default;
        virtual ~AssetDataBase() = default;

    protected:
        virtual void _VInit(const HE::Char* in_szName, const Core::File::Path& in_rPath);
        virtual HE::Bool _VLoad(Platform::FileInterface&) = 0;
        virtual void _VUnload()                           = 0;

    protected:
        Core::Common::FixedString128 _szName;
        Core::File::Path _path;
    };

    /// <summary>
    /// ツリーのノード抽象
    /// </summary>
    class AbstractTreeNode
    {
    public:
        static constexpr HE::Sint32 sNoneLevel = -1;

    public:
        AbstractTreeNode(const HE::UTF8* in_szKey, const HE::Sint32 in_sLevel)
        {
            this->_szKey  = in_szKey;
            this->_sLevel = in_sLevel;
        }

        virtual ~AbstractTreeNode() = default;

        const HE::UTF8* Name() const { return this->_szKey.Str(); }
        const HE::Sint32 GetLevel() const { return this->_sLevel; }

        HE::Bool IsNone() const { return (this->_sLevel == sNoneLevel); }

        virtual void* VGetSource()                                        = 0;
        virtual HE::Uint32 VGetUInt32(const HE::UTF8* in_szName = NULL)   = 0;
        virtual HE::Sint32 VGetSInt32(const HE::UTF8* in_szName = NULL)   = 0;
        virtual HE::Float32 VGetFloat32(const HE::UTF8* in_szName = NULL) = 0;
        virtual void VOutputString(Core::Common::StringBase* out,
                                   const HE::UTF8* in_szName = NULL)      = 0;
        virtual const HE::Bool VOutputUTF8(HE::UTF8* out, const HE::Uint32 in_uLen,
                                           const HE::UTF8* in_szName)     = 0;

    protected:
        HE::Sint32 _sLevel = sNoneLevel;
        Core::Common::FixedString64 _szKey;
    };

    /// <summary>
    /// アセットデータのTreeノード型への各プロパティを取得インターフェイス
    /// </summary>
    class InterfaceTreeData
    {
    public:
        using NodeSharedPtr = Core::Memory::SharedPtr<AbstractTreeNode>;

    public:
        // ノード名を複数指定してルートから探索してノード取得
        virtual NodeSharedPtr VGetNodeByName(
            const std::initializer_list<const HE::UTF8*>& in_aName) = 0;

        // 引数指定したノードを起点としてノード名を探索してノード取得
        virtual NodeSharedPtr VGetNodeByName(
            AbstractTreeNode& in_rLocateNode,
            const std::initializer_list<const HE::UTF8*>& in_aName) = 0;

        // 引数ノードの一階層下の子ノードを取得
        // レベル指定して子階層内の子ノードを選択して取得
        // in_rCuurentNode
        // |
        // | - child_node_1 -> レベル0
        // | - child_node_2 -> レベル1
        virtual NodeSharedPtr VGetNodeByLevel(
            const std::initializer_list<const HE::UTF8*>& in_aName, const HE::Sint32 in_uLevel) = 0;

        virtual NodeSharedPtr VGetNodeByLevel(AbstractTreeNode& in_rLocateNode,
                                              const HE::Sint32 in_uLevel) = 0;
    };

}  // namespace AssetManager
