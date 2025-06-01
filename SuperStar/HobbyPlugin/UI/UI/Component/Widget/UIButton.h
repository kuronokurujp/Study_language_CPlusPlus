#pragma once

#include <functional>
#include <memory>

#include "Engine/Math/Rect2.h"
#include "Engine/MiniEngine.h"
#include "UIWidget.h"

namespace UI
{
    class UIButtonComponent;

    /// <summary>
    /// ボタンのプッシュ通知ハンドラー
    /// </summary>
    class UIButtonMessageHandler
    {
        HE_CLASS_COPY_NG(UIButtonMessageHandler);

    public:
        UIButtonMessageHandler()          = default;
        virtual ~UIButtonMessageHandler() = default;

        void OnPush(UIButtonComponent* in_pBtnComp) { this->_VOnPushInternal(in_pBtnComp); }

    protected:
        virtual void _VOnPushInternal(UIButtonComponent* in_bBtnComp) = 0;
    };

    using UIButtonMessageHandlerImpOnPush = std::function<void(UIButtonComponent*)>;

    /// <summary>
    /// ボタンのプッシュ通知
    /// デフォルト版
    /// </summary>
    class UIButtonMessageHandlerDefault final : public UIButtonMessageHandler
    {
        HE_CLASS_COPY_NG(UIButtonMessageHandlerDefault);

    public:
        // UIButtonMessageHandlerDefault() = default;
        UIButtonMessageHandlerDefault(UIButtonMessageHandlerImpOnPush in_func)
            : _onPush(std::move(in_func))
        {
        }

    protected:
        void _VOnPushInternal(
            UIButtonComponent* in_pBtnComp) override final;  // { this->_onPush(this->_szMsg); }

    private:
        UIButtonMessageHandlerImpOnPush _onPush;
        // Core::Common::FixedString128 _szMsg;
    };

    /// <summary>
    /// UIに表示するボタンコンポーネント
    /// タッチイベントはコンポーネントUIInputTerminalTouchで呼び出している
    /// </summary>
    class UIButtonComponent final : public UIWidgetComponent
    {
        HE_CLASS_COPY_NG(UIButtonComponent);
        HE_GENERATED_CLASS_BODY_HEADER(UIButtonComponent, UIWidgetComponent);

    public:
        UIButtonComponent(Core::Memory::UniquePtr<
                          UIButtonMessageHandler>);  // : UIWidgetComponent() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void VSetup(const HE::Bool in_bAutoDelete = TRUE) override;

        /// <summary>
        /// コンポーネントの終了
        /// </summary>
        virtual HE::Bool VEnd() override;

        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        void VUpdate(const HE::Float32) override;

        /// <summary>
        /// プッシュ通知のハンドラーを設定
        /// ユニークポインタで所有権を移譲している
        /// </summary>
        /*
        void SetPushHandler(Core::Memory::UniquePtr<UIButtonMessageHandler> in_spHandler)
        {
            this->_pushHandler = std::move(in_spHandler);
        }
        */

        // void SetWidth(const HE::Float32 in_fW) { this->_fWidth = in_fW; }
        // void SetHeight(const HE::Float32 in_fH) { this->_fHeight = in_fH; }
        void SetAnchor(const Core::Math::EAnchor in_eAnchor) { this->_eAnchor = in_eAnchor; }

        /// <summary>
        /// タッチイベント
        /// </summary>
        void VOnTouch(const EnhancedInput::InputData::Item::Touch& in_rTouch) override;

    private:
        void _Clear()
        {
            UIWidgetComponent::_Clear();

            this->_pushHandler.release();
            //  this->_fWidth  = 0.0f;
            //  this->_fHeight = 0.0f;
            this->_eAnchor = Core::Math::EAnchor_Left;
        }

    private:
        Core::Memory::UniquePtr<UIButtonMessageHandler> _pushHandler;
        //  HE::Float32 _fWidth          = 0.0f;
        //  HE::Float32 _fHeight         = 0.0f;
        Core::Math::EAnchor _eAnchor = Core::Math::EAnchor_Left;
    };
}  // namespace UI
