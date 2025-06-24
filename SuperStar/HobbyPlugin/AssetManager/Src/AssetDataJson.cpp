#include "AssetManager/AssetDataJson.h"

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

// サードパーティーのjsonライブラリ
#include "simidjson/simdjson.h"

namespace AssetManager
{
    namespace Local
    {
        /// <summary>
        /// ノード
        /// </summary>
        class JsonNode : public AbstractTreeNode
        {
        public:
            JsonNode(const HE::UTF8* in_szKey, simdjson::dom::element& in_rValue,
                     const HE::Sint32 in_sLevel)
                : AbstractTreeNode(in_szKey, in_sLevel), _value(in_rValue)
            {
            }

            void* VGetSource() override { return &this->_value; }

            HE::Uint32 VGetUInt32(const HE::UTF8* in_szName = NULL) override
            {
                simdjson::dom::element element = this->_value;
                if (in_szName != NULL)
                {
                    element = this->_value[in_szName];
                }

                HE_ASSERT(element.is_number());
                return static_cast<HE::Uint32>(element.get_int64().value_unsafe());
            }

            HE::Sint32 VGetSInt32(const HE::UTF8* in_szName = NULL) override
            {
                return static_cast<HE::Sint32>(this->VGetUInt32(in_szName));
            }

            HE::Float32 VGetFloat32(const HE::UTF8* in_szName = NULL) override
            {
                simdjson::dom::element element = this->_value;
                if (in_szName != NULL)
                {
                    element = this->_value[in_szName];
                }

                return static_cast<HE::Float32>(element.get_double().value_unsafe());
            }

            void VOutputString(Core::Common::StringBase* out,
                               const HE::UTF8* in_szName = NULL) override

            {
                simdjson::dom::element element = this->_value;
                if (in_szName != NULL)
                {
                    element = this->_value[in_szName];
                }

                auto val = element.get_string();
                if (val.error() == FALSE)
                {
                    // unsafeの方が高速なのだが, 文字列の中にゴミの値が入っていた
                    *out = val.value().data();
                }
                else
                {
                    out->Clear();
                }
            }

            const HE::Bool VOutputUTF8(HE::UTF8* out, const HE::Uint32 in_uLen,
                                       const HE::UTF8* in_szName) override
            {
                simdjson::dom::element element = this->_value;
                if (in_szName != NULL)
                {
                    element = this->_value[in_szName];
                }

                auto val = element.get_string();
                if (val.error() == FALSE)
                {
                    HE_STR_U8_COPY_S(out, in_uLen, val.value().data(),
                                     HE_STR_U8_LENGTH(val.value().data()));
                    return TRUE;
                }
                return FALSE;
            }

        public:
            simdjson::dom::element _value;
        };

        static std::tuple<const Core::Common::FixedString128, simdjson::dom::element, HE::Sint32>
        GetValueBySimdJson(simdjson::dom::element& in_rDoc,
                           const std::initializer_list<const HE::UTF8*>& in_rTokens)
        {
            HE_ASSERT(0 < in_rTokens.size());

            HE::Sint32 sLevel = AbstractTreeNode::sNoneLevel;
            simdjson::simdjson_result<simdjson::dom::element> result;
            simdjson::dom::element element;
            try
            {
                auto v = in_rDoc.get_object();

                const HE::UTF8* szKey = *in_rTokens.begin();
                for (auto itr = in_rTokens.begin(); itr != in_rTokens.end(); ++itr)
                {
                    result     = v[*itr];
                    auto error = result.error();
                    if (error == simdjson::error_code::SUCCESS)
                    {
                        szKey   = *itr;
                        element = result.value();
                        sLevel  = 0;

                        v = element.get_object();
                    }
                    else
                    {
                        sLevel = AbstractTreeNode::sNoneLevel;
                        HE_PG_LOG_LINE(HE_STR_TEXT("error(%d): Token(%s)"), error, *itr);
                        break;
                    }
                }

                return std::tuple<const Core::Common::FixedString128, simdjson::dom::element,
                                  HE::Sint32>(szKey, element, sLevel);
            }
            catch (const simdjson::simdjson_error& e)
            {
                sLevel = AbstractTreeNode::sNoneLevel;
                HE_PG_LOG_LINE(HE_STR_TEXT("json要素がない: %s"), e.what());
            }

            return std::tuple<const Core::Common::FixedString128, simdjson::dom::element,
                              HE::Sint32>(NULL, element, AbstractTreeNode::sNoneLevel);
        }
    }  // namespace Local

    InterfaceTreeData::NodeSharedPtr AssetDataJson::VGetNodeByName(
        const std::initializer_list<const HE::UTF8*>& in_aName)
    {
        auto pJson   = reinterpret_cast<simdjson::padded_string*>(this->_pJson);
        auto pParser = reinterpret_cast<simdjson::dom::parser*>(this->_pParser);
        simdjson::simdjson_result<simdjson::dom::element> dom = pParser->parse(*pJson);

        auto [szKey, value, sLevel] = Local::GetValueBySimdJson(dom.value(), in_aName);

        return HE_MAKE_CUSTOM_SHARED_PTR((Local::JsonNode), szKey.Str(), value, sLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataJson::VGetNodeByName(
        AbstractTreeNode& in_rCurrentNode, const std::initializer_list<const HE::UTF8*>& in_aName)
    {
        auto pElement = reinterpret_cast<simdjson::dom::element*>(in_rCurrentNode.VGetSource());

        auto szKey        = *in_aName.begin();
        auto currentPath  = *pElement;
        HE::Sint32 sLevel = AbstractTreeNode::sNoneLevel;

        for (auto it = in_aName.begin(); it != in_aName.end(); ++it)
        {
            auto checkPath = currentPath[*it].value();
            if (checkPath.type() != simdjson::dom::element_type::NULL_VALUE)
            {
                currentPath = checkPath;
                szKey       = *it;
                sLevel      = 0;
            }
            else
            {
                break;
            }
        }

        return HE_MAKE_CUSTOM_SHARED_PTR((Local::JsonNode), szKey, currentPath, sLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataJson::VGetNodeByLevel(
        const std::initializer_list<const HE::UTF8*>& in_aName, const HE::Sint32 in_uLevel)
    {
        auto pJson   = reinterpret_cast<simdjson::padded_string*>(this->_pJson);
        auto pParser = reinterpret_cast<simdjson::dom::parser*>(this->_pParser);
        simdjson::simdjson_result<simdjson::dom::element> dom = pParser->parse(*pJson);

        auto [szKey, val, sLevel] = Local::GetValueBySimdJson(dom.value(), in_aName);

        Local::JsonNode node(szKey.Str(), val, sLevel);
        return this->VGetNodeByLevel(node, in_uLevel);
    }

    InterfaceTreeData::NodeSharedPtr AssetDataJson::VGetNodeByLevel(
        AbstractTreeNode& in_rCurrentNode, const HE::Sint32 in_uLevel)
    {
        if (in_rCurrentNode.IsNone())
        {
            simdjson::dom::element element;
            return HE_MAKE_CUSTOM_SHARED_PTR((Local::JsonNode), "", element,
                                             AbstractTreeNode::sNoneLevel);
        }

        auto pNodeSource = reinterpret_cast<simdjson::dom::element*>(in_rCurrentNode.VGetSource());

        HE::Sint32 sLevel = AbstractTreeNode::sNoneLevel;

        std::string_view key;
        simdjson::dom::element value;

        for (auto& field : pNodeSource->get_object())
        {
            if (in_uLevel <= sLevel) break;

            key   = field.key;
            value = field.value;

            ++sLevel;
        }

        // レベル外のノードはなし
        if (sLevel < in_uLevel)
        {
            sLevel = AbstractTreeNode::sNoneLevel;
        }

        return HE_MAKE_CUSTOM_SHARED_PTR((Local::JsonNode), key.data(), value, sLevel);
    }

    HE::Bool AssetDataJson::IsToken(const std::initializer_list<const HE::UTF8*>& in_rTokens)
    {
        auto pJson   = reinterpret_cast<simdjson::padded_string*>(this->_pJson);
        auto pParser = reinterpret_cast<simdjson::dom::parser*>(this->_pParser);
        simdjson::simdjson_result<simdjson::dom::element> dom = pParser->parse(*pJson);

        auto [szKey, val, sLevel] = Local::GetValueBySimdJson(dom.value(), in_rTokens);

        if (sLevel == AbstractTreeNode::sNoneLevel) return FALSE;

        return TRUE;
    }

    HE::Bool AssetDataJson::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        HE::Bool bRet = TRUE;

        auto [pText, uSize] = in_rFileSystem.VLoadText(this->_path);
        try
        {
            // 読み込んだメモリをjsonデータとして展開
            // 展開時にjsonを展開するためのメモリ確保をする
            HE_ASSERT(simdjson::validate_utf8(pText, uSize));

            this->_pJson   = HE_NEW_MEM(simdjson::padded_string, 0)(pText, uSize);
            this->_pParser = HE_NEW_MEM(simdjson::dom::parser, 0)(uSize * 2);
            {
                auto pJson = reinterpret_cast<simdjson::padded_string*>(this->_pJson);
                if (pJson->length() <= 0)
                {
                    bRet = FALSE;

                    HE_PG_LOG_LINE(HE_STR_TEXT("%s ファイルエラー"), this->_path.Str());
                    HE_LOG_LINE(HE_STR_TEXT("エラーのjson内容"));
                    HE_LOG_LINE(HE_STR_TEXT("%s"), pText);
                }
            }
        }
        catch (const simdjson::simdjson_error& e)
        {
            HE_PG_LOG_LINE(HE_STR_TEXT("%s ファイルの扱いに失敗: %s"), this->_path.Str(), e.what());
            bRet = FALSE;
        }
        HE_SAFE_DELETE_MEM(pText);

        return bRet;
    }

    void AssetDataJson::_VUnload()
    {
        HE_SAFE_DELETE_MEM(this->_pParser);
        HE_SAFE_DELETE_MEM(this->_pJson);
    }

}  // namespace AssetManager
