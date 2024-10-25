#include "InGameRendererUserShipComponent.h"

// 利用するモジュール一覧
#include "RenderModule.h"

namespace InGame
{
    void InGameRendererUserShipComponent::SetViewHandle(Core::Common::Handle& in_rHandle)
    {
        this->_viewHandle = in_rHandle;
    }

    void InGameRendererUserShipComponent::SetTransformHandle(Core::Common::Handle& in_rHandle)
    {
        this->_transformHandle = in_rHandle;
    }

    void InGameRendererUserShipComponent::SetSize(Core::Math::Vector2& in_rSize)
    {
        this->_size = in_rSize;
    }

    Bool InGameRendererUserShipComponent::VBegin()
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

    void InGameRendererUserShipComponent::VUpdate(const Float32 in_fDt)
    {
        Actor::Component::VUpdate(in_fDt);

        // 描画座標取得
        Core::Math::Rect2 rect;
        {
            auto pTrans =
                this->Owner()->GetComponent<Actor::TransformComponent>(this->_transformHandle);
            HE_ASSERT(pTrans);

            Core::Math::Rect2 srcRect(0.0f, 0.0f, this->_size._fX, this->_size._fY,
                                      Core::Math::Rect2::EAnchor_Center);
            pTrans->TransformLocalToWorldRect2D(&rect, srcRect);
        }

        // 描画コマンド追加
        // TODO: 画像表示に切り替えるかも
        Render::Command2DRectDraw(this->_viewHandle, rect, Render::RGB::White);
    }

}  // namespace InGame
