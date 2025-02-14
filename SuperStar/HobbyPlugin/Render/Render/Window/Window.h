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

        using OnBeginCallback = std::function<void(Core::Common::Handle)>;
        using OnEndCallback   = std::function<void(Core::Common::Handle)>;

    public:
        /// <summary>
        /// ウィンドウ座標指定
        /// </summary>
        void SetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY);

        // ビューポート関連
        Core::Common::Handle AddViewPort(Core::Memory::UniquePtr<Platform::ViewPortStrategy>);
        HE::Bool RemoveViewPort(Core::Common::Handle&);

        /// <summary>
        /// 利用準備完了しているか
        /// </summary>
        const HE::Bool IsReady() const { return this->_bReady; }

        /// <summary>
        /// ウィンドウ表示
        /// </summary>
        void Show();

        // イベント関連
        void SetBeginRenderCallback(OnRenderBeginCallback);
        void SetEndRenderCallback(OnRenderEndCallback);

#ifdef HE_USE_SDL2
        void* GetWindowBySDL2() const;
        void* GetContentBySDL2() const;
#endif

    private:
        HE::Bool Init(Core::Memory::UniquePtr<Platform::WindowStrategy>, OnBeginCallback,
                      OnEndCallback);
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
        HE::Uint32 _uX, _uY = HE::uInvalidUint32;

        OnRenderBeginCallback _onRenderBegin = NULL;
        OnRenderEndCallback _onRenderEnd     = NULL;
        OnBeginCallback _onBegin             = NULL;
        OnEndCallback _onEnd                 = NULL;
    };

}  // namespace Render
