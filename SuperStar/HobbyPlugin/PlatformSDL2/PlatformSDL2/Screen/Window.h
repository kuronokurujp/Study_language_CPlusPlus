#pragma once

#include "Engine/MiniEngine.h"

// 依存モジュール一覧
#include "RenderModule.h"

namespace PlatformSDL2
{
    /// <summary>
    /// DXLibのウィンドウロジック
    /// </summary>
    class DXLibWindowStrategy final : public Render::WindowStrategy
    {
    public:
        DXLibWindowStrategy();

    protected:
        void _VBegin() override final;
        void _VEnd() override final;

        void _VShow() override final;

        void _VBeginRender() override final;
        void _VEndRender() override final;

    private:
        void* _pWindow = NULL;
        void* _pContext = NULL;
    };

}  // namespace PlatformSDL2
