#pragma once

#include "Engine/MiniEngine.h"

// 依存モジュール一覧
#include "RenderModule.h"

namespace PlatformSDL2
{
    /// <summary>
    /// 2D用のシーン
    /// </summary>
    class SceneView2D : public Render::SceneViewBase
    {
    protected:
        HE::Bool _VBegin() override final;
        void _VEnd() override final;

        void _VUpdate(const HE::Float32) override final;

        void _VRender(Render::ViewPort* in_pViewPort) override final;

    private:
        // 2Dの点群を表示するために必要
        static constexpr HE::Uint32 s_u2DPointCount = 500;
        // static inline ::POINTDATA s_a2DPoint[s_u2DPointCount];
    };

    /// <summary>
    /// UI用のシーン
    /// </summary>
    class SceneViewUI final : public SceneView2D
    {
    };

}  // namespace PlatformSDL2
