#pragma once

#include <functional>

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
        using OnRenderBeginCallback = std::function<void()>;
        using OnRenderEndCallback   = std::function<void()>;

    public:
        Core::Common::Handle AddViewPort(Core::Memory::UniquePtr<Platform::ViewPortStrategy>);
        HE::Bool RemoveViewPort(Core::Common::Handle&);

        /// <summary>
        /// ウィンドウ表示
        /// </summary>
        void Show();

        // レンダリング関連のイベント
        // TODO: 追加という名前になっているが, 現在は一つしか追加できない
        // 後々追加に変更対応
        void AddBeginRenderCallback(OnRenderBeginCallback);
        void AddEndRenderCallback(OnRenderEndCallback);

#ifdef HE_USE_SDL2
        void* GetWindowBySDL2() const;
        void* GetContentBySDL2() const;
#endif

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

        OnRenderBeginCallback _onRenderBegin = NULL;
        OnRenderEndCallback _onRenderEnd     = NULL;
    };

}  // namespace Render
