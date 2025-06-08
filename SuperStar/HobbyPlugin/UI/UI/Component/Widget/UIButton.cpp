#include "UIButton.h"

#include "Engine/Math/Vector3.h"

// 依存するモジュール一覧
// #include "ActorModule.h"
#include "RenderModule.h"

namespace UI
{
    UIButtonComponent::UIButtonComponent(
        Core::Memory::UniquePtr<UIButtonMessageHandler> in_uHandler)
        : UIWidgetComponent()
    {
        this->_Clear();

        this->_pushHandler = std::move(in_uHandler);
    }

    void UIButtonComponent::VSetup(const HE::Bool in_bAutoDelete)
    {
        UIWidgetComponent::VSetup(in_bAutoDelete);
    }

    /// <summary>
    /// ボタンコンポーネントが終了
    /// </summary>
    /// <returns></returns>
    HE::Bool UIButtonComponent::VEnd()
    {
        const HE::Bool bRet = UIWidgetComponent::VEnd();

        // プッシュハンドルを解放
        HE_SAFE_DELETE_UNIQUE_PTR(this->_pushHandler);

        return bRet;
    }

    /// <summary>
    /// コンポーネントの更新
    /// 必ず処理を記述
    /// </summary>
    void UIButtonComponent::VUpdate(const HE::Float32 in_fDeltaTime)
    {
        UIWidgetComponent::VUpdate(in_fDeltaTime);

        Core::Math::Rect2 srcRect;
        srcRect.SetPosition(0.0f, 0.0f, this->_size._fX, this->_size._fY, this->_eAnchor);

        Core::Math::Rect2 rect;

        auto pTrans = this->GetTransformComponent();
        HE_ASSERT(pTrans);

        pTrans->TransformLocalToWorldRect2D(&rect, srcRect);

        // 描画コマンドを追加
        Render::Command2DQuadDraw(this->_viewHandle, rect, {this->_uColor});
    }

    void UIButtonComponent::VOnTouch(const HE::Char* in_szInputName,
                                     const EnhancedInput::InputData::Item::Touch& in_rTouch)
    {
        Core::Math::Rect2 rect;
        Core::Math::Rect2 orgRect;
        orgRect.SetPosition(0.0f, 0.0f, this->_size._fX, this->_size._fY, this->_eAnchor);

        auto pTrans = this->GetTransformComponent();
        HE_ASSERT(pTrans);

        pTrans->TransformLocalToWorldRect2D(&rect, orgRect);

        if (rect.InSidePoint(Core::Math::Vector2(in_rTouch._fX, in_rTouch._fY)))
        {
            this->_pushHandler->OnPush(this);
        }
    }

    void UIButtonMessageHandlerDefault::_VOnPushInternal(UIButtonComponent* in_pBtnComp)
    {
        this->_onPush(in_pBtnComp);
    }

}  // namespace UI
