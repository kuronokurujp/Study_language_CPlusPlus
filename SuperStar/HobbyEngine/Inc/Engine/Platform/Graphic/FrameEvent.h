#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/Graphic/Drawable.h"

namespace Platform
{
    /// <summary>
    /// グラフィックの基本フレームイベントのインタフェース
    /// </summary>
    class EventFrameInterface
    {
    public:
        virtual void VDraw(MapDrawable&) = 0;
    };
}  // namespace Platform
