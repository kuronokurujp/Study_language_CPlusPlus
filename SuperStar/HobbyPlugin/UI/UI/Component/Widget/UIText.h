#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/Math/Rect2.h"
#include "UIWidget.h"

namespace UI
{
    /// <summary>
    /// UIに表示するテキストコンポーネント
    /// </summary>
    class UITextComponent final : public UIWidgetComponent
    {
        HE_CLASS_COPY_NG(UITextComponent);
        HE_GENERATED_CLASS_BODY_HEADER(UITextComponent, UIWidgetComponent);

    public:
        using ConvLocTextFuncType =
            std::function<const HE::Char*(const HE::Char*, const HE::Char*)>;

    public:
        UITextComponent(ConvLocTextFuncType);

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void VSetup(const HE::Bool in_bAutoDelete) override;

        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        void VUpdate(const HE::Float32 in_fDeltaTime) override;

        /// <summary>
        /// テキスト設定
        /// </summary>
        inline void SetText(const HE::Char* in_szText) { this->_szText = in_szText; }

        /// <summary>
        /// ローカライズグループ名
        /// </summary>
        inline void SetLocGroupName(const HE::Char* in_szName) { this->_szLocGroup = in_szName; }

        /// <summary>
        /// テキストの描画矩形を設定
        /// </summary>
        // inline void SetRect(const Core::Math::Rect2& in_rRect) { this->_rect = in_rRect; }

        /// <summary>
        /// ローカライズグループ名を設定
        /// 文字列がある == ローカライズテキスト
        /// </summary>
        inline void SetLogGroup(const HE::Char* in_szGroupName)
        {
            this->_szLocGroup = in_szGroupName;
        }

        inline void SetAnchor(const Core::Math::EAnchor in_eAnchor) { this->_eAnchor = in_eAnchor; }

        inline void SetFontSize(const HE::Uint32 in_uFontSize) { this->_uFontSize = in_uFontSize; }

    private:
        void _Clear()
        {
            UIWidgetComponent::_Clear();

            this->_szText.Clear();
            this->_szDrawText.Clear();
            this->_szLocGroup.Clear();
            // this->_rect.Clear();
            this->_eAnchor         = Core::Math::EAnchor_Left;
            this->_convLocTextFunc = NULL;
        }

    private:
        Core::Common::FixedString1024 _szText;
        Core::Common::FixedString1024 _szDrawText;
        Core::Common::FixedString128 _szLocGroup;
        // Core::Math::Rect2 _rect;
        Core::Math::EAnchor _eAnchor = Core::Math::EAnchor_Left;
        HE::Uint32 _uFontSize        = 32;

        ConvLocTextFuncType _convLocTextFunc;
    };
}  // namespace UI
