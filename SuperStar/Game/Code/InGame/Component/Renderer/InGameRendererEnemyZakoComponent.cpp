#include "InGameRendererEnemyZakoComponent.h"

// 利用するモジュール一覧
#include "RenderModule.h"

namespace InGame
{
    void InGameRendererEnemyZakoComponent::VSetSize(const Core::Math::Vector2& in_rSize)
    {
        this->_size = in_rSize;
    }

    void InGameRendererEnemyZakoComponent::_VRenderer(const Core::Common::Handle& in_rViewHandle,
                                                      Actor::TransformComponent* in_pTrans)
    {
        // 描画座標取得
        Core::Math::Rect2 rect;
        {
            Core::Math::Rect2 srcRect;
            srcRect.SetPosition(0.0f, 0.0f, this->_size._fX, this->_size._fY,
                                Core::Math::EAnchor_Center);
            in_pTrans->TransformLocalToWorldRect2D(&rect, srcRect);
        }

        // 描画コマンド追加
        // TODO: 画像表示に切り替えるかも
        Render::Command2DQuadDraw(in_rViewHandle, rect, Core::Math::RGB::White);
    }

}  // namespace InGame
