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
        HE::Uint32 _uX, _uY = HE::uInvalidUint32;
    };

}  // namespace Render
