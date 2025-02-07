#include "InGameRendererUserShipComponent.h"

// 利用するモジュール一覧
#include "RenderModule.h"

namespace InGame
{
    InGameRendererUserShipComponent::InGameRendererUserShipComponent(
        const Core::Common::Handle& in_rViewHandle)
        : InGameRendererBaseComponent()
    {
        this->SetViewHandle(in_rViewHandle);
    }

    void InGameRendererUserShipComponent::VSetSize(const Core::Math::Vector2& in_rSize)
    {
        this->_size = in_rSize;
    }

    void InGameRendererUserShipComponent::_VRenderer(const Core::Common::Handle& in_rViewHandle,
                                                     Actor::TransformComponent* in_pTrans)
    {
        // 描画座標取得
        Core::Math::Vector2 pos;
        {
            in_pTrans->TransformLocalToWorldPos2D(&pos, Core::Math::Vector2::Zero);
        }

        // 描画コマンド追加
        Render::Command2DTriangleDraw(in_rViewHandle, pos, Core::Math::EAnchor_Center, -90,
                                      this->_size._fX, Core::Math::RGB::White);
    }
}  // namespace InGame
