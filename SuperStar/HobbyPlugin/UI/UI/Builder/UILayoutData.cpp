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
        constexpr HE::UTF8* s_locGroupDelimita = HE_STR_TEXT(".");

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

        static const EAnchor GetPosAnchor(AssetManager::AbstractTreeNode& in_rNode)
        {
            in_rNode.VOutputString(&Core::Common::g_szTempFixedString1024,
                                   HE_STR_U8_TEXT("anchor"));
            if (Core::Common::g_szTempFixedString1024.Empty()) return EAnchor_Left;

            if (Core::Common::g_szTempFixedString1024.Find(HE_STR_TEXT("center")) == 0)
            {
                return EPosAnchor_Center;
            }

            return EAnchor_Left;
        }

        static void ParseStyle(Style* out, HE::UTF8* in_szStyle)
        {
            HE_ASSERT_RETURN(out);
            HE_ASSERT_RETURN(in_szStyle);
            {
                out->_fH     = 0.0f;
                out->_fW     = 0.0f;
                out->_uColor = 0;
                out->_uSize  = 32;
            }

            HE::UTF8* pRestartToken = NULL;
            HE::UTF8* pToken        = ::strtok_s(in_szStyle, ";", &pRestartToken);

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
                        out->_fW = HE_U8_TO_FLOAT32(pValue);
                    }
                    else if (::strcmp(szKey, "h") == 0)
                    {
                        out->_fH = HE_U8_TO_FLOAT32(pValue);
                    }
                    else if (::strcmp(szKey, "color") == 0)
                    {
                        HE::UTF8 szColorName[128] = {0};
                        ::strncpy_s(szColorName, HE_ARRAY_NUM(szColorName), pValue,
                                    HE_ARRAY_NUM(szColorName));
                        // TODO: キーワードと色名の対応ハッシュテーブルを作る
                        HE::Bool bColorName = FALSE;
                        for (HE::Uint32 i = 0; i < HE_ARRAY_NUM(s_aColorTable); ++i)
                        {
                            if (::strcmp(s_aColorTable[i].szName, szColorName) == 0)
                            {
                                out->_uColor = s_aColorTable[i].rgba.c;
                                bColorName   = TRUE;
                                break;
                            }
                        }

                        if (bColorName == FALSE)
                        {
                            // カラー名がないので16進数数字と判断して数値に変換
                            out->_uColor = HE_U8_HEX_TO_UINT32(szColorName);
#ifdef HE_LITTLE_ENDIAN
                            HE_SWAP_BYTE_32BIT(out->_uColor);
#endif
                        }
                    }
                    else if (::strcmp(szKey, "size") == 0)
                    {
                        out->_uSize = HE_U8_TO_UINT32(pValue);
                    }
                }

                pToken = ::strtok_s(NULL, ";", &pRestartToken);
            }
        }

        static void ApplyNode(Node* out, AssetManager::AbstractTreeNode& in_rNode)
        {
            HE_ASSERT(out);
            ::memset(out, 0, sizeof(Node));

            Node::Data* pData = &out->_data;

            // TODO: ノードから反映する情報を抜き出す
            pData->_eWidgetType = UI::Builder::EWidget_None;

            Core::Common::FixedString128 szAttrName = in_rNode.Name();
            if (szAttrName == HE_STR_U8_TEXT("ui"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Root;
            }
            else if (szAttrName == HE_STR_U8_TEXT("widget"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Widget;
                auto pWidget        = &pData->_exData._widget;
                pWidget->_fX        = in_rNode.VGetFloat32({HE_STR_U8_TEXT("x")});
                pWidget->_fY        = in_rNode.VGetFloat32({HE_STR_U8_TEXT("y")});
            }
            else if (szAttrName == HE_STR_U8_TEXT("btn"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Button;
                auto pBtn           = &pData->_exData.button;
                pBtn->_fX           = in_rNode.VGetFloat32({HE_STR_U8_TEXT("x")});
                pBtn->_fY           = in_rNode.VGetFloat32({HE_STR_U8_TEXT("y")});
                pBtn->_eAnchor      = GetPosAnchor(in_rNode);
                // インプットのキーを取得
                in_rNode.VOutputUTF8(pBtn->_szInputKey, HE_ARRAY_SIZE(pBtn->_szInputKey),
                                     HE_STR_U8_TEXT("input"));

                in_rNode.VOutputUTF8(Core::Common::g_szTempFixedUTF8_1024,
                                     HE_ARRAY_SIZE(Core::Common::g_szTempFixedUTF8_1024),
                                     HE_STR_U8_TEXT("style"));
                ParseStyle(&pBtn->_style, Core::Common::g_szTempFixedUTF8_1024);
            }
            else if (szAttrName == HE_STR_U8_TEXT("label"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Label;
                auto pLabel         = &pData->_exData._label;
                pLabel->_fX         = in_rNode.VGetFloat32({HE_STR_U8_TEXT("x")});
                pLabel->_fY         = in_rNode.VGetFloat32({HE_STR_U8_TEXT("y")});
                pLabel->_eAnchor    = GetPosAnchor(in_rNode);

                // ローカライズテキストか
                pLabel->bLoc = (in_rNode.VGetUInt32({HE_STR_U8_TEXT("loc")}) != 0);

                if (in_rNode.VOutputUTF8(Core::Common::g_szTempFixedUTF8_1024,
                                         HE_ARRAY_SIZE(Core::Common::g_szTempFixedUTF8_1024),
                                         HE_STR_U8_TEXT("text")))
                {
                    if (pLabel->bLoc)
                    {
                        // 文字列を分割してローカライズ用のデータを構築
                        Core::Common::SplitWithUTF8(
                            // 分割されたトークンを受け取る
                            [pLabel](const HE::UTF8* in_szToken, const HE::Uint32 in_uTokenCount)
                            {
                                switch (in_uTokenCount)
                                {
                                    case 1:
                                    {
                                        HE_STR_COPY_S(pLabel->szLoc, HE_ARRAY_NUM(pLabel->szLoc),
                                                      in_szToken, HE_STR_U8_LENGTH(in_szToken));

                                        break;
                                    }
                                    case 2:
                                    {
                                        HE_STR_COPY_S(pLabel->szText, HE_ARRAY_NUM(pLabel->szText),
                                                      in_szToken, HE_STR_U8_LENGTH(in_szToken));

                                        break;
                                    }
                                    default:
                                        break;
                                }
                            },
                            Core::Common::g_szTempFixedUTF8_1024,
                            HE_STR_U8_LENGTH(Core::Common::g_szTempFixedUTF8_1024),
                            s_locGroupDelimita);
                    }
                    else
                    {
                        HE_STR_COPY_S(pLabel->szText, HE_ARRAY_NUM(pLabel->szText),
                                      Core::Common::g_szTempFixedUTF8_1024,
                                      HE_STR_U8_LENGTH(Core::Common::g_szTempFixedUTF8_1024));
                    }
                }

                in_rNode.VOutputUTF8(Core::Common::g_szTempFixedUTF8_1024,
                                     HE_ARRAY_SIZE(Core::Common::g_szTempFixedUTF8_1024),
                                     HE_STR_U8_TEXT("style"));

                ParseStyle(&pLabel->_style, Core::Common::g_szTempFixedUTF8_1024);
            }
            else if (szAttrName == HE_STR_U8_TEXT("layer"))
            {
                pData->_eWidgetType = UI::Builder::EWidget_Layer;
                auto pLayer         = &pData->_exData._layer;
                pLayer->_fX         = in_rNode.VGetFloat32({HE_STR_U8_TEXT("x")});
                pLayer->_fY         = in_rNode.VGetFloat32({HE_STR_U8_TEXT("y")});

                in_rNode.VOutputUTF8(Core::Common::g_szTempFixedUTF8_1024,
                                     HE_ARRAY_SIZE(Core::Common::g_szTempFixedUTF8_1024),
                                     HE_STR_U8_TEXT("style"));

                ParseStyle(&pLayer->_style, Core::Common::g_szTempFixedUTF8_1024);
            }

            {
                auto szIdName = HE_STR_U8_TEXT("id");
                auto bRet =
                    in_rNode.VOutputUTF8(pData->_szId, HE_ARRAY_SIZE(pData->_szId), szIdName);
                HE_ASSERT(bRet && "文字列のコピーでエラー");
            }
        }
    }  // namespace Local

    HE::Bool UILayoutData::OutputNode(Node* out,
                                      const std::initializer_list<const HE::UTF8*>& in_aName)
    {
        HE_ASSERT(out);
        auto rrLibNode = this->VGetNodeByName(in_aName);

        Local::ApplyNode(out, *rrLibNode);

        return TRUE;
    }

    void UILayoutData::DoSearchHierarchyNode(
        const std::initializer_list<const HE::UTF8*>& in_aName,
        std::function<void(const Node&, const HE::Uint32, const HE::Bool)> in_result)
    {
        HE::Sint32 sRootLevel = 0;
        Node out;
        HE::Uint32 uSort = 0;
        HE::Bool bRoot   = TRUE;
        Core::Common::FixedStack<AssetManager::InterfaceTreeData::NodeSharedPtr, 128> sStack;
        while (1)
        {
            auto spParentNode = this->VGetNodeByLevel(in_aName, sRootLevel);
            if (spParentNode->IsNone()) break;

            Local::ApplyNode(&out, *spParentNode);
            in_result(out, uSort, bRoot);
            bRoot = FALSE;

            // ノードの下に全ての入れ子を再帰処理で取得して処理する
            HE::Sint32 sChildLevel = 0;
            auto spChildNode       = this->VGetNodeByLevel(*spParentNode, sChildLevel);
            if (spChildNode->IsNone() == FALSE)
            {
                spParentNode = spChildNode;

                ++uSort;

                while (spParentNode->IsNone() == FALSE)
                {
                    Local::ApplyNode(&out, *spParentNode);
                    in_result(out, uSort, bRoot);

                    auto spNextNode = this->VGetNodeByLevel(*spParentNode, sChildLevel);
                    sChildLevel     = spNextNode->GetLevel() + 1;

                    if (spNextNode->IsNone())
                    {
                        // 一つ上の階層のノードがあれば階層を一つ前に戻す
                        if (0 < sStack.Size())
                        {
                            spParentNode = *sStack.PushBack();
                            sChildLevel  = spParentNode->GetLevel() + 1;

                            --uSort;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        // 現在の階層のノードを保存
                        sStack.PushBack(spParentNode);
                        // 階層を下へ移動
                        spParentNode = spNextNode;

                        sChildLevel = 0;
                        ++uSort;
                    }
                }
            }

            ++sRootLevel;
            uSort = 0;
            bRoot = TRUE;
        }
    }

}  // namespace UI::Builder
