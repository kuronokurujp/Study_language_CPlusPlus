#pragma once

#include "AssetDataBase.h"
#include "Engine/Common/CustomMap.h"

//  tomlライブラリ内の例外処理をOFF
//  こちらでエラー処理をする
#define TOML_EXCEPTIONS 0
#include "AssetManager/ThirdParty/tomlplusplus/toml.hpp"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

namespace AssetManager
{
    /// <summary>
    /// tomlアセットデータ
    /// </summary>
    class AssetDataToml : public AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataToml);
        HE_CLASS_MOVE_NG(AssetDataToml);

    public:
        // 前方宣言
        class Node;

        // Nodeをマップで扱う型
        using ToolNodeMapType = Core::Common::FixedMap<Core::Common::FixedString128, Node, 64>;

        class Node final
        {
        public:
            Node() {}
            Node(toml::node_view<toml::node> in_node) : _node(in_node) {}
            Node(toml::node& in_rNode) : _node(in_rNode) {}

            const Core::Common::FixedString512 GetString() const;

            template <typename... TArgs>
            typename std::enable_if<(std::is_same<TArgs, const HE::Char*>::value && ...), Node>::type
            GetNode(TArgs... args)
            {
                // 引数の個数を取得
                HE::Uint32 uCount = static_cast<HE::Uint32>(sizeof...(args));
                if (uCount <= 0) return Node();

                // 初期化リストを使用して引数を処理
                const HE::Char* values[] = {args...};

                return this->_GetNode(values, uCount);
            }

            template <typename... TArgs>
            typename std::enable_if<(std::is_same<TArgs, const HE::Char*>::value && ...),
                                    const HE::Bool>::type
            OutputNodeMap(ToolNodeMapType* out, TArgs... args)
            {
                // 引数の個数を取得
                HE::Uint32 uCount = static_cast<HE::Uint32>(sizeof...(args));
                if (uCount <= 0) return FALSE;

                // 初期化リストを使用して引数を処理
                const HE::Char* szaName[] = {args...};
                return this->_OutputNodeMap(out, szaName, uCount);
            }

        private:
            HE::Bool _OutputNodeMap(ToolNodeMapType* out, const HE::Char* in_szaName[],
                                const HE::Uint32 in_uCount);
            Node _GetNode(const HE::Char* in_szaName[], HE::Uint32 in_uCount);

        private:
            toml::node_view<toml::node> _node;
        };

        AssetDataToml() : AssetDataBase() {}

        virtual HE::Bool _VLoad(Platform::FileInterface&) override;
        virtual void _VUnload() override;

        Node GetRootNode();

    private:
        toml::parse_result _result;
    };
}  // namespace AssetManager
