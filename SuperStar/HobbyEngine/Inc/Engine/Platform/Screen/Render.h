#pragma once

#include "Engine/MiniEngine.h"

namespace Platform
{
    /// <summary>
    /// レンダリングのインターフェイス
    /// </summary>
    class RenderInterface
    {
        HE_GENERATED_CLASS_BASE_BODY_HEADER(RenderInterface);

    public:
        RenderInterface()       = default;
        virtual void VStart()   = 0;
        virtual void VRelease() = 0;
    };
}  // namespace Platform
