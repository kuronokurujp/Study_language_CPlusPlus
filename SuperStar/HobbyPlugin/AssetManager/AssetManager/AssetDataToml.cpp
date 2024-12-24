#include "AssetDataToml.h"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    HE::Bool AssetDataToml::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        auto [szText, uFileSize] = in_rFileSystem.VLoadText(this->_path);
        this->_pText = szText;

        // ファイルロード
        // this->_result = toml::parse_file(this->_path.Str());
        std::string_view st(this->_pText);
        std::string_view stFilePath(this->_path.Str());
        this->_result = toml::parse(st, stFilePath);
        if (this->_result.failed())
        {
            // エラーログを出してアサートで止める
            Core::Common::FixedString256 errorMsg(this->_result.error().description().data());
            HE_LOG_LINE(HE_STR_TEXT("%s"), errorMsg.Str());
            HE_ASSERT(FALSE);
            HE_SAFE_DELETE_MEM(this->_pText);

            return FALSE;
        }

        return TRUE;
    }

    void AssetDataToml::_VUnload()
    {
        HE_SAFE_DELETE_MEM(this->_pText);
    }

    AssetDataToml::Node AssetDataToml::GetRootNode()
    {
        toml::node& rNode = this->_result.table();
        return Node(rNode);
    }

    const Core::Common::FixedString512 AssetDataToml::Node::GetString() const
    {
        std::optional<std::string_view> s = this->_node.value<std::string_view>();
        return Core::Common::FixedString512(s->data());
    }

    AssetDataToml::Node AssetDataToml::Node::_GetNode(const HE::Char* in_szaName[],
                                                      const HE::Uint32 in_uCount)
    {
        toml::node_view<toml::node> node = this->_node;
        for (HE::Uint32 i = 0; i < in_uCount; ++i)
        {
            node = node.at_path(in_szaName[i]);
            if (node == FALSE) return Node();
        }

        return Node(node);
    }

    HE::Bool AssetDataToml::Node::_OutputNodeMap(ToolNodeMapType* out, const HE::Char* in_szaName[],
                                                 const HE::Uint32 in_uCount)
    {
        HE_ASSERT(out && "出力するポインターがNULL");
        HE_ASSERT(0 < in_uCount);

        toml::node_view<toml::node> node = this->_node;
        for (HE::Uint32 i = 0; i < in_uCount; ++i)
        {
            if (HE_STR_CMP(in_szaName[i], HE_STR_EMPTY) == 0) break;

            node = node.at_path(in_szaName[i]);
            if (node == FALSE) return FALSE;
        }

        // 指定したノードの中に複数のノードがあれば設定する
        if (node.is_table() == FALSE) return FALSE;

        auto table = node.as_table();
        for (auto it = table->begin(); it != table->end(); ++it)
        {
            out->Add(it->first.data(), Node(it->second));
        }

        return TRUE;
    }

}  // namespace AssetManager
