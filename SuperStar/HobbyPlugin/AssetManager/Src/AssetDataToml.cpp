#include "AssetManager/AssetDataToml.h"

//  tomlライブラリ内の例外処理をOFF
//  こちらでエラー処理をする
#define TOML_EXCEPTIONS 0
#include "tomlplusplus/toml.hpp"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    namespace Local
    {
        class TomlNode : public AbstractTreeNode
        {
        public:
            TomlNode(toml::node_view<toml::node> in_node, const HE::UTF8* in_szKey,
                     const HE::Sint32 in_sLevel)
                : AbstractTreeNode(in_szKey, in_sLevel), _value(in_node)
            {
            }

            void* VGetSource() override { return reinterpret_cast<void*>(this->_value.node()); }

            HE::Uint32 VGetUInt32(const HE::UTF8* in_szName = NULL) override
            {
                toml::node* pRefNode = this->_value.node();
                if (in_szName != NULL)
                {
                    auto node = pRefNode->at_path(in_szName);
                    pRefNode  = node.node();
                }

                return static_cast<HE::Uint32>(pRefNode->as_integer()->get());
            }

            HE::Sint32 VGetSInt32(const HE::UTF8* in_szName = NULL) override
            {
                return static_cast<HE::Sint32>(this->VGetUInt32(in_szName));
            }

            HE::Float32 VGetFloat32(const HE::UTF8* in_szName = NULL) override
            {
                toml::node* pRefNode = this->_value.node();
                if (in_szName != NULL)
                {
                    auto node = pRefNode->at_path(in_szName);
                    pRefNode  = node.node();
                }

                return static_cast<HE::Uint32>(pRefNode->value<HE::Float32>().value());
            }

            void VOutputString(Core::Common::StringBase* out,
                               const HE::UTF8* in_szName = NULL) override
            {
                toml::node* pRefNode = this->_value.node();
                if (in_szName != NULL)
                {
                    auto node = pRefNode->at_path(in_szName);
                    pRefNode  = node.node();
                }

                if (pRefNode->is_string())
                {
                    auto str = pRefNode->as_string()->get();
                    *out     = str.c_str();
                }
                else
                {
                    out->Clear();
                }
            }

            const HE::Bool VOutputUTF8(HE::UTF8* out, const HE::Uint32 in_uLen,
                                       const HE::UTF8* in_szName) override
            {
                toml::node* pRefNode = this->_value.node();
                if (in_szName != NULL)
                {
                    auto node = pRefNode->at_path(in_szName);
                    pRefNode  = node.node();
                }

                if (pRefNode->is_string())
                {
                    auto str = pRefNode->as_string()->get();
                    HE_STR_U8_COPY_S(out, in_uLen, str.c_str(), HE_STR_U8_LENGTH(str.c_str()));
                    return TRUE;
                }

                return FALSE;
            }

        public:
            toml::node_view<toml::node> _value;
        };

        static std::tuple<const toml::node_view<toml::node>, const HE::UTF8*, const HE::Sint32>
        GetTomlNode(toml::node& in_rTomlTable,
                    const std::initializer_list<const HE::UTF8*>& in_aName)
        {
            toml::node_view<toml::node> hitNode(in_rTomlTable);

            auto szName       = *in_aName.begin();
            HE::Sint32 sLevel = AbstractTreeNode::sNoneLevel;
            for (auto it = in_aName.begin(); it != in_aName.end(); ++it)
            {
                auto checkNode = hitNode.at_path(*it);
                if (checkNode == FALSE) break;

                hitNode = checkNode;
                szName  = *it;
                ++sLevel;
            }

            return std::tuple<const toml::node_view<toml::node>, const HE::UTF8*, const HE::Sint32>(
                hitNode, szName, sLevel);
        }

    }  // namespace Local

    HE::Bool AssetDataToml::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        auto [szText, uFileSize] = in_rFileSystem.VLoadText(this->_path);
        this->_pText             = szText;

        // ファイルロード
        std::string_view st(this->_pText);
        std::string_view stFilePath(this->_path.Str());

        this->_pResult = HE_NEW_MEM(toml::parse_result, 0)(toml::parse(st, stFilePath));
        auto pResult   = reinterpret_cast<toml::parse_result*>(this->_pResult);
        if (pResult->failed())
        {
            // エラーログを出してアサートで止める
            Core::Common::FixedString256 szErrorMsg(pResult->error().description().data());
            HE_LOG_LINE(HE_STR_TEXT("%s"), szErrorMsg.Str());
            HE_ASSERT(FALSE);
            HE_SAFE_DELETE_MEM(this->_pText);

            return FALSE;
        }

        return TRUE;
    }

    void AssetDataToml::_VUnload()
    {
        HE_SAFE_DELETE_MEM(this->_pResult);
        HE_SAFE_DELETE_MEM(this->_pText);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataToml::VGetNodeByName(
        const std::initializer_list<const HE::UTF8*>& in_aName)
    {
        auto pResult               = reinterpret_cast<toml::parse_result*>(this->_pResult);
        auto [node, szKey, sLevel] = Local::GetTomlNode(pResult->table(), in_aName);

        return HE_MAKE_CUSTOM_SHARED_PTR((Local::TomlNode), node, szKey, sLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataToml::VGetNodeByName(
        AbstractTreeNode& in_rCurrentNode, const std::initializer_list<const HE::UTF8*>& in_aName)
    {
        toml::node* pNode = reinterpret_cast<toml::node*>(in_rCurrentNode.VGetSource());
        HE_ASSERT(pNode->is_table());

        toml::node* pCurrentNode = pNode->as_table();

        const HE::UTF8* szKey = NULL;
        HE::Sint32 sLevel     = AbstractTreeNode::sNoneLevel;
        for (auto it = in_aName.begin(); it != in_aName.end(); ++it)
        {
            auto checkNode = pCurrentNode->at_path(*it);
            if (checkNode.is_table())
            {
                szKey        = *it;
                pCurrentNode = checkNode.as_table();
                sLevel       = 0;
            }
            else
            {
                szKey        = *it;
                pCurrentNode = checkNode.node();
                break;
            }
        }

        toml::node_view<toml::node> nodeView{pCurrentNode};
        return HE_MAKE_CUSTOM_SHARED_PTR((Local::TomlNode), nodeView, szKey, sLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataToml::VGetNodeByLevel(
        const std::initializer_list<const HE::UTF8*>& in_aName, const HE::Sint32 in_uLevel)
    {
        auto spNode = this->VGetNodeByName(in_aName);
        return this->VGetNodeByLevel(*spNode, in_uLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataToml::VGetNodeByLevel(
        AbstractTreeNode& in_rCurrentNode, const HE::Sint32 in_uLevel)
    {
        toml::node_view<toml::node> nodeView{};
        if (in_rCurrentNode.IsNone())
        {
            return HE_MAKE_CUSTOM_SHARED_PTR((Local::TomlNode), nodeView, HE_STR_U8_TEXT(""),
                                             AbstractTreeNode::sNoneLevel);
        }

        toml::node* pNode = reinterpret_cast<toml::node*>(in_rCurrentNode.VGetSource());
        if (pNode->is_table() == FALSE)
        {
            return HE_MAKE_CUSTOM_SHARED_PTR((Local::TomlNode), nodeView, HE_STR_U8_TEXT(""),
                                             AbstractTreeNode::sNoneLevel);
        }

        auto pTable       = pNode->as_table();
        HE::Sint32 sLevel = AbstractTreeNode::sNoneLevel;

        toml::key key;
        for (auto& obj : *pTable)
        {
            if (in_uLevel <= sLevel) break;

            toml::node_view<toml::node> view{obj.second};
            nodeView = view;

            key = obj.first;

            ++sLevel;
        }

        // レベル外のノードはなし
        if (sLevel < in_uLevel)
        {
            sLevel = AbstractTreeNode::sNoneLevel;
        }

        return HE_MAKE_CUSTOM_SHARED_PTR((Local::TomlNode), nodeView, key.data(), sLevel);
    }

}  // namespace AssetManager
