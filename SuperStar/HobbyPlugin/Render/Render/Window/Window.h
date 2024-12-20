#pragma once

#include "./ViewPort.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/MiniEngine.h"

namespace Platform
{
    class WindowStrategy;
    class ViewPortStrategy;

}  // namespace Platform

namespace Render
{
    /// <summary>
    /// 描画するウィンドウ
    /// </summary>
    class Window final
    {
        friend class RenderModule;

    public:
        Core::Common::Handle AddViewPort(Core::Memory::UniquePtr<Platform::ViewPortStrategy>);
        HE::Bool RemoveViewPort(Core::Common::Handle&);

        /// <summary>
        /// ウィンドウ表示
        /// </summary>
        void Show();

    private:
        HE::Bool Init(Core::Memory::UniquePtr<Platform::WindowStrategy>);
        void Release();

        void _Begin();
        void _End();

        void _Update(const HE::Float32);

        void _Render();

    private:
        Core::Memory::UniquePtr<Platform::WindowStrategy> _upStrategy;
        Core::Common::RuntimePoolManager<ViewPort> _poolViewPortManager;

        HE::Bool _bShow  = FALSE;
        HE::Bool _bReady = FALSE;
    };

}  // namespace Render
