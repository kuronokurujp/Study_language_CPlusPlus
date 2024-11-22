#pragma once

#include "Engine/MiniEngine.h"
#include "InGameRendererBaseComponent.h"

namespace InGame
{
    /// <summary>
    /// ユーザー船のレンダリングコンポーネント
    /// </summary>
    class InGameRendererUserShipComponent final : public InGameRendererBaseComponent
    {
        HE_CLASS_COPY_NG(InGameRendererUserShipComponent);
        HE_CLASS_MOVE_NG(InGameRendererUserShipComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameRendererUserShipComponent,
                                       InGameRendererBaseComponent);

    public:
        InGameRendererUserShipComponent(const Core::Common::Handle& in_rViewHandle);

        void SetSize(const Core::Math::Vector2&);

    protected:
        void _VRenderer(const Core::Common::Handle& in_rViewHandle,
                        class Actor::TransformComponent*) override final;

    private:
        Core::Math::Vector2 _size;
    };

}  // namespace InGame
