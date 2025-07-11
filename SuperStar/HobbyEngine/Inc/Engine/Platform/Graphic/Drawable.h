#pragma once

#include "Engine/MiniEngine.h"

namespace Platform
{
    /// <summary>
    /// 描画のインターフェイス
    /// </summary>
    class DrawableInterface
    {
        HE_GENERATED_CLASS_BASE_BODY_HEADER(DrawableInterface);

    public:
        DrawableInterface()   = default;
        virtual void VBegin() = 0;
        virtual void VEnd()   = 0;

        virtual void VPreDraw()  = 0;
        virtual void VPostDraw() = 0;
    };

    /// <summary>
    /// 2D描画のインターフェイス
    /// </summary>
    class Drawable2DInterface : public DrawableInterface
    {
        HE_GENERATED_CLASS_BODY_HEADER(Drawable2DInterface, DrawableInterface);

    public:
        Drawable2DInterface() = default;

        virtual void VSetViewSize(const HE::Float32 in_fW, const HE::Float32 in_fH) = 0;
    };

    using MapDrawable = Core::Common::FixedMap<const Core::Common::RTTI*,
                                               Core::Memory::UniquePtr<Drawable2DInterface>, 32>;
}  // namespace Platform
