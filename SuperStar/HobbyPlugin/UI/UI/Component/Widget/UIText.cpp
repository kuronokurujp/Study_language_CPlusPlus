#include "UIText.h"

#include "Engine/Engine.h"

// 依存するモジュール一覧
#include "RenderModule.h"

namespace UI
{
    UITextComponent::UITextComponent(ConvLocTextFuncType in_func) : UIWidgetComponent()
    {
        this->_Clear();

        this->_convLocTextFunc = std::move(in_func);
    }

    void UITextComponent::VSetup(const HE::Bool in_bAutoDelete)
    {
        UIWidgetComponent::VSetup(in_bAutoDelete);
    }

    void UITextComponent::VUpdate(const HE::Float32 in_fDeltaTime)
    {
        UIWidgetComponent::VUpdate(in_fDeltaTime);

        // ローカライズテキストならローカライズテキストを取得
        if (0 < this->_szLocGroup.Size() && this->_convLocTextFunc)
        {
            // 言語切り替えが必要
            this->_szDrawText =
                this->_convLocTextFunc(this->_szLocGroup.Str(), this->_szText.Str());
        }
        else
        {
            this->_szDrawText = this->_szText;
        }

        Core::Math::Vector2 pos;

        auto pTrans = this->GetTransformComponent();

        HE_ASSERT(pTrans);

        Core::Math::Rect2 rect;
        rect.SetPosition(0, 0, this->_size._fX, this->_size._fY, this->_eAnchor);
        pTrans->TransformLocalToWorldPos2D(&pos, rect.Pos());

        Render::Command2DTextDraw(this->_viewHandle, pos, this->_szDrawText, this->_uFontSize,
                                  {this->_uColor}, this->_eAnchor, 2.0f);
    }
}  // namespace UI
