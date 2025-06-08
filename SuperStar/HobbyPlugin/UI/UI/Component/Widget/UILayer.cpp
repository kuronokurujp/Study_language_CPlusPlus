#include "UILayer.h"

// 依存するモジュール一覧
#include "RenderModule.h"

namespace UI
{
    void UILayerComponent::VUpdate(const HE::Float32 in_fDt)
    {
        UIWidgetComponent::VUpdate(in_fDt);

        // サイズがあれば矩形描画
        if (Core::Math::NearZero(this->_size._fX)) return;
        if (Core::Math::NearZero(this->_size._fY)) return;

        auto pTrans = this->GetTransformComponent();
        HE_ASSERT(pTrans);

        Core::Math::Rect2 srcRect;
        srcRect.SetPosition(0.0f, 0.0f, this->_size._fX, this->_size._fY, Core::Math::EAnchor_Left);

        Core::Math::Rect2 rect;

        pTrans->TransformLocalToWorldRect2D(&rect, srcRect);

        Render::Command2DQuadDraw(this->_viewHandle, rect, {this->_uColor});
    }
}  // namespace UI
