#include "PlatformScreen.h"

namespace GameDevGUI
{
    void ScreenRender::VCls(const HE::Uint32 in_uR, const HE::Uint32 in_uG, const HE::Uint32 in_uB)
    {
    }

    void ScreenRender::V2DDrawPartical(const Platform::ViewPortConfig& in_rViewConfig,
                                       const Core::Common::Handle in_rParticleHandle,
                                       const Core::Math::Vector3&)
    {
    }

    void ScreenRender::V2DDrawText(const Platform::ViewPortConfig& in_rViewConfig,
                                   const Core::Math::Vector2& in_rPos,
                                   const Core::Math::EAnchor in_eAnchor, const HE::Char* in_szText,
                                   const HE::Uint32 in_uTextSize, const HE::Uint32 in_uSpace,
                                   const Core::Math::Color in_color)
    {
    }

    void ScreenRender::V2DDrawQuad(const Platform::ViewPortConfig& in_rViewConfig,
                                   const Core::Math::Rect2& in_rRect2D,
                                   const Core::Math::Color in_color)
    {
    }

    void ScreenRender::V2DDrawCircle(const Platform::ViewPortConfig& in_rViewConfig,
                                     const Core::Math::Vector2& in_rPos,
                                     const Core::Math::EAnchor in_eAchor,
                                     const HE::Float32 in_fSize, const Core::Math::Color in_color)
    {
    }

    void ScreenRender::V2DDrawTriangle(const Platform::ViewPortConfig& in_rViewConfig,
                                       const Core::Math::Vector2& in_rPos,
                                       const Core::Math::EAnchor in_eAchor,
                                       const HE::Float32 in_rAngleDegress,
                                       const HE::Float32 in_fSize, const Core::Math::Color)
    {
    }
}  // namespace GameDevGUI
