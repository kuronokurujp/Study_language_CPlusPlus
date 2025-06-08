#pragma once

// UILayoutのデータファイルからUILayoutを構築するデータを生成
#include <functional>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomStack.h"
#include "Engine/Common/CustomString.h"

// 依存モジュール
#include "AssetManagerModule.h"

namespace UI::Builder
{
    enum EAnchor
    {
        EAnchor_Left = 0,
        EPosAnchor_Center,
        EPosAnchor_Max
    };

    enum EWidget
    {
        EWidget_None = 0,
        EWidget_Root,
        EWidget_Widget,
        EWidget_Label,
        EWidget_Button,
        EWidget_Layer,
    };

    struct Style
    {
        HE::Float32 _fW, _fH;
        HE::Uint32 _uSize;
        HE::Uint32 _uColor;
    };

    struct Node
    {
        /// <summary>
        /// エンジン用のデータ
        /// </summary>
        struct Data
        {
            HE::UTF8 _szId[64]   = {0};
            EWidget _eWidgetType = EWidget::EWidget_None;

            union ExData
            {
                struct Widget
                {
                    HE::Float32 _fX, _fY;
                } _widget;

                struct Label
                {
                    EAnchor _eAnchor;
                    HE::Bool bLoc;
                    HE::Float32 _fX, _fY;
                    HE::UTF8 szLoc[64];
                    HE::UTF8 szText[64];
                    Style _style;
                } _label;

                struct Button
                {
                    EAnchor _eAnchor;
                    HE::Float32 _fX, _fY;
                    // インプットのキーを追加
                    HE::UTF8 _szInputKey[32];
                    Style _style;
                } button;

                struct Layout
                {
                    HE::Float32 _fX, _fY;
                    Style _style;
                } _layer;
            } _exData;
        } _data;
    };

    /// <summary>
    /// UILayoutのデータファイルからLayoutを構築するデータを生成
    /// </summary>
    class UILayoutData final : public AssetManager::AssetDataXml
    {
        HE_CLASS_COPY_NG(UILayoutData);
        HE_CLASS_MOVE_NG(UILayoutData);

    public:
        UILayoutData() : AssetManager::AssetDataXml() {}

        /// <summary>
        /// 指定したノード下にあるノードを名前で取得
        /// </summary>
        HE::Bool OutputNode(Node* out, const std::initializer_list<const HE::UTF8*>& in_aName);

        /// <summary>
        /// 再帰処理をノードの全入れ子のパラメータを呼び出す
        /// </summary>
        void DoSearchHierarchyNode(
            const std::initializer_list<const HE::UTF8*>& in_aName,
            std::function<void(const Node&, const HE::Uint32, const HE::Bool)> in_result);
    };
}  // namespace UI::Builder
