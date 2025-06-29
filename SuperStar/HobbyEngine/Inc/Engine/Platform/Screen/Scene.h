#pragma once

#include "Engine/MiniEngine.h"

namespace Platform
{
    struct SceneConfig
    {
    };

    /// <summary>
    /// シーンの基本ロジック
    /// </summary>
    class SceneStrategy
    {
    public:
        SceneStrategy(const SceneConfig&);
        virtual ~SceneStrategy() = default;

        virtual HE::Bool VBegin()               = 0;
        virtual void VUpdate(const HE::Float32) = 0;
        virtual void VEnd()                     = 0;

        virtual void VBeginRender() = 0;
        virtual void VEndRender()   = 0;
    };

}  // namespace Platform
