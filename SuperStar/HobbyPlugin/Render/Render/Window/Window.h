#pragma once

#include "./ViewPort.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/MiniEngine.h"

namespace Render
{
    /// <summary>
    /// ウィンドウのロジック
    /// </summary>
    class WindowStrategy
    {
        friend class Window;

    public:
        inline HE::Uint32 ViewPortCount() const { return this->_uViewPortCount; }

    protected:
        virtual void _VBegin() = 0;
        virtual void _VEnd()   = 0;

        virtual void _VShow() = 0;

        virtual void _VBeginRender() = 0;
        virtual void _VEndRender()   = 0;

    protected:
        HE::Uint32 _uViewPortCount = 0;
        HE::Uint32 _uWidht         = 0;
        HE::Uint32 _uHeight        = 0;
    };

    /// <summary>
    /// 描画するウィンドウ
    /// </summary>
    class Window final
    {
        friend class RenderModule;

    public:
        Core::Common::Handle AddViewPort(Core::Memory::UniquePtr<ViewPortConfig>);
        HE::Bool RemoveViewPort(const Core::Common::Handle&);

        Core::Memory::SharedPtr<ViewPort> GetViewPort(const Core::Common::Handle&);

        /// <summary>
        /// ウィンドウ表示
        /// </summary>
        void Show();

    private:
        HE::Bool _Setup(Core::Memory::UniquePtr<WindowStrategy>);
        void _Release();

        void _Begin();
        void _End();

        void _Update(const HE::Float32);

        void _Render();

    private:
        Core::Memory::UniquePtr<WindowStrategy> _upStrategy;
        Core::Common::RuntimePoolManager<ViewPort> _poolViewPortManager;

        HE::Bool _bShow  = FALSE;
        HE::Bool _bReady = FALSE;
    };

}  // namespace Render
