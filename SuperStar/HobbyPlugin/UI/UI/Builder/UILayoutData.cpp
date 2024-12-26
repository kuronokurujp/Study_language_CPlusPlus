#include "UILayoutData.h"

#include <string.h>

#include "Engine/Common/CustomArray.h"
#include "Engine/Common/CustomVector.h"

// 依存モジュール
#include "RenderModule.h"

namespace UI::Builder
{
    /// <summary>
    /// ローカル内のみ使用
    /// </summary>
    namespace Local
    {
        constexpr HE::Char s_treeNodeDelimita = HE_STR_TEXT('.');
        constexpr HE::Char s_locGroupDelimita = HE_STR_TEXT('.');

        /// <summary>
        /// UIのスタイルカラー
        /// </summary>
        struct StyleColor
        {
        public:
            StyleColor(const HE::UTF8* in_szName, Core::Math::Color in_rgba)
            {
                this->szName = in_szName;
                this->rgba   = in_rgba;
            }

            const HE::UTF8* szName = NULL;
            Core::Math::Color rgba;
        };

        /// <summary>
        /// UIデータの色名とカラーと対応したテーブル一覧
        /// </summary>
        static const StyleColor s_aColorTable[] = {StyleColor("white", Core::Math::RGB::White),
                                                   StyleColor("black", Core::Math::RGB::Black),
                                                   StyleColor("red", Core::Math::RGB::Red),
                                                   StyleColor("blue", Core::Math::RGB::Blue),
                                                   StyleColor("green", Core::Math::RGB::Green),
                                                   StyleColor("yellow", Core::Math::RGB::Yellow),
                                                   StyleColor("cyan", Core::Math::RGB::Cyan),
                                                   StyleColor("magenta", Core::Math::RGB::Magenta),
                                                   StyleColor("ornage", Core::Math::RGB::Orange)};

        static const EAnchor GetPosAnchor(const pugi::xml_node& in_rNode)
        {
            const pugi::char_t* s = in_rNode.attribute("anchor").as_string();
            if (s == NULL) return EAnchor_Left;

            if (::strcmp(s, "center") == 0)
            {
                return EPosAnchor_Center;
            }

            return EAnchor_Left;
        }

        static void ParseStyle(Style* out, const HE::UTF8* in_szName, const HE::Uint32 in_uSize)
        {
            static HE::UTF8 buff[1024] = {0};

            HE_ASSERT(in_uSize < HE_ARRAY_SIZE(buff));
            {
                out->_fH     = 0.0f;
                out->_fW     = 0.0f;
                out->_uColor = 0;
                out->_uSize  = 32;
            }

            ::memcpy_s(buff, HE_ARRAY_SIZE(buff), in_szName, in_uSize);
            HE::UTF8* pRestartToken = NULL;
            HE::UTF8* pToken        = ::strtok_s(buff, ";", &pRestartToken);

            while (pToken != NULL)
            {
                HE::UTF8* szKey  = ::strtok(pToken, ":");
                HE::UTF8* pValue = ::strtok(NULL, ":");

                if (szKey && pValue)
                {
                    while (*szKey == ' ') ++szKey;
                    while (*pValue == ' ') ++pValue;

                    if (::strcmp(szKey, "w") == 0)
                    {
                        out->_fW = static_cast<HE::Float32>(::atof(pValue));
                    }
                    else if (::strcmp(szKey, "h") == 0)
                    {
                        out->_fH = static_cast<HE::Float32>(::atoi(pValue));
                    }
                    else if (::strcmp(szKey, "color") == 0)
                    {
                        HE::UTF8 szColorName[128] = {0};
                        ::strncpy_s(szColorName, HE_ARRAY_NUM(szColorName), pValue,
                                    HE_ARRAY_NUM(szColorName));
                        // TODO: キーワードと色名の対応ハッシュテーブルを作る
                        for (HE::Uint32 i = 0; i < HE_ARRAY_NUM(s_aColorTable); ++i)
                        {
                            if (::strcmp(s_aColorTable[i].szName, szColorName) == 0)
                            {
                                out->_uColor = s_aColorTable[i].rgba.c;
                                break;
                            }
                        }
                    }
                    else if (::strcmp(szKey, "size") == 0)
                    {
                        out->_uSize = static_cast<HE::Uint32>(::atoi(pValue));
                    }
                }

                pToken = ::strtok_s(NULL, ";", &pRestartToken);
            }
        }

        static void ApplyNode(Node* out, const pugi::xml_node& in_rNode)
        {
            HE_ASSERT(out);
            ::memset(out, 0, sizeof(Node));

            // サードパーティライブラリのノードポインタを保存
            out->_pNode = in_rNode.internal_object();

            Node::Data* pData = &out->_data;

            // TODO: ノードから反映する情報を抜き出す
            pData->_eWidgetType = UI::Builder::EWidget_None;

            Core::Common::FixedString128 szAttrName = in_rNode.name();
            if (szAttrName == HE_STR_TEXT("ui"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Root;
            }
            else if (szAttrName == HE_STR_TEXT("w"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Widget;
                auto pWidget        = &pData->_exData._widget;
                pWidget->_fX        = in_rNode.attribute("x").as_float();
                pWidget->_fY        = in_rNode.attribute("y").as_float();
            }
            else if (szAttrName == HE_STR_TEXT("b"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Button;
                auto pBtn           = &pData->_exData.button;
                pBtn->_fX           = in_rNode.attribute("x").as_float();
                pBtn->_fY           = in_rNode.attribute("y").as_float();
                pBtn->_eAnchor      = GetPosAnchor(in_rNode);

                auto s = in_rNode.attribute("style").value();
                ParseStyle(&pBtn->_style, s, static_cast<HE::Uint32>(::strlen(s)));
            }
            else if (szAttrName == HE_STR_TEXT("t"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Label;
                auto pLabel         = &pData->_exData._label;
                pLabel->_fX         = in_rNode.attribute("x").as_float();
                pLabel->_fY         = in_rNode.attribute("y").as_float();
                pLabel->_eAnchor    = GetPosAnchor(in_rNode);

                // ローカライズテキストか
                if (in_rNode.attribute("loc").as_bool())
                {
                    pLabel->bLoc = TRUE;
                }

                auto t = in_rNode.attribute("text").value();
                if (0 < ::strlen(t))
                {
                    Core::Common::g_szTempFixedString1024 = t;
                    if (pLabel->bLoc)
                    {
                        Core::Common::FixedArray<Core::Common::FixedString1024, 3> aSplitName;

                        Core::Common::OutputSplitString(aSplitName,
                                                        Core::Common::g_szTempFixedString1024,
                                                        s_locGroupDelimita);
                        HE_STR_CPY_S(pLabel->szLoc, HE_ARRAY_NUM(pLabel->szLoc),
                                     aSplitName[0].Str(), aSplitName[0].Size());

                        HE_STR_CPY_S(pLabel->szText, HE_ARRAY_NUM(pLabel->szText),
                                     aSplitName[1].Str(), aSplitName[1].Size());
                    }
                    else
                    {
                        HE_STR_CPY_S(pLabel->szText, HE_ARRAY_NUM(pLabel->szText),
                                     Core::Common::g_szTempFixedString1024.Str(),
                                     Core::Common::g_szTempFixedString1024.Size());
                    }
                }

                auto s = in_rNode.attribute("style").value();
                ParseStyle(&pLabel->_style, s, static_cast<HE::Uint32>(::strlen(s)));
            }
            else if (szAttrName == HE_STR_TEXT("l"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Layout;
                auto pLayout        = &pData->_exData._layout;

                auto s = in_rNode.attribute("style").value();
                ParseStyle(&pLayout->_style, s, static_cast<HE::Uint32>(::strlen(s)));
            }

            Core::Common::FixedString1024 szIdName(in_rNode.attribute("id").value());
            HE_STR_ERRNO e = HE_STR_CPY_S(pData->_szId, HE_ARRAY_NUM(pData->_szId), szIdName.Str(),
                                          szIdName.Size());
            HE_ASSERT(HE_STR_SUCCESS(e) && "文字列のコピーでエラー");
        }
    }  // namespace Local

    HE::Bool UILayoutData::OutputNodeByRootPos(Node* out, const HE::UTF8* in_szName)
    {
        HE_ASSERT(out);

        Core::Common::FixedString256 nodeName(in_szName);
        HE::Uint64 ulTargetNodeHash = nodeName.Hash();

        auto libNode = this->_doc.child(in_szName);
        HE_ASSERT(libNode.empty() == FALSE && "ノードがない");

        Local::ApplyNode(out, libNode);

        return TRUE;
    }

    HE::Bool UILayoutData::OutputNode(Node* out, const Node& in_rParentNode,
                                      const HE::UTF8* in_szName)
    {
        HE_ASSERT(out);
        HE_ASSERT(in_rParentNode._pNode);

        pugi::xml_node libNode(in_rParentNode._pNode);
        libNode = libNode.child(in_szName);

        Local::ApplyNode(out, libNode);

        return TRUE;
    }

    void UILayoutData::OutputNodeChildren(Core::Common::StackBase<Node>* out,
                                          const Node& in_rParentNode)
    {
        HE_ASSERT(out);

        pugi::xml_node libNode(in_rParentNode._pNode);
        {
            for (auto b = libNode.begin(); b != libNode.end(); ++b)
            {
                Node node;
                Local::ApplyNode(&node, *b);
                out->PushBack(node);
            }
        }
    }

}  // namespace UI::Builder
