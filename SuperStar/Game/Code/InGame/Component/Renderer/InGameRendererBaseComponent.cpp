#include "InGameRendererBaseComponent.h"

#include "Engine/Engine.h"

// 利用するモジュール一覧
#include "RenderModule.h"

namespace InGame
{
    void InGameRendererBaseComponent::SetViewHandle(const Core::Common::Handle& in_rHandle)
    {
        this->_viewHandle = in_rHandle;
    }

    void InGameRendererBaseComponent::SetTransformHandle(const Core::Common::Handle& in_rHandle)
    {
        this->_transformHandle = in_rHandle;
    }

    HE::Bool InGameRendererBaseComponent::VBegin()
    {
        if (Actor::Component::VBegin() == FALSE) return FALSE;

        // 座標情報を持つコンポーネントのハンドルを取得
        // このコンポーネントがないと処理できない
        auto [h, c] =
            this->Owner()->GetComponentHandleAndComponent(&Actor::TransformComponent::CLASS_RTTI);
        HE_ASSERT(h.Null() == FALSE);
        this->_transformHandle = h;

        return TRUE;
    }

    void InGameRendererBaseComponent::VUpdate(const HE::Float32 in_fDt)
    {
        Actor::Component::VUpdate(in_fDt);

        auto pTrans =
            this->Owner()->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);
        this->_VRenderer(this->_viewHandle, pTrans);
    }

    HE::Bool InGameRendererBaseComponent::IsScreenIn()
    {
        auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
        auto pViewport     = pRenderModule->GetViewPort(this->_viewHandle);

        auto pTrans =
            this->Owner()->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);

        auto rSize = this->VGetSize();
        auto pos   = pTrans->GetWorldPos();

        // 描画の矩形を生成
        Core::Math::Rect2 renderRect;
        renderRect.SetPosition(pos._fX, pos._fY, rSize._fX, rSize._fY, Core::Math::EAnchor_Center);

        // ビューポートの矩形を生成
        Core::Math::Rect2 viewportRect;
        viewportRect.SetPosition(0, 0, pViewport->Width(), pViewport->Height(),
                                 Core::Math::EAnchor_Left);

        // ビューポートの矩形が描画の矩形に納まっていれば画面内とする
        return (viewportRect.InSideRect(renderRect));
    }

}  // namespace InGame
