#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"

// プラットフォームの描画に関わる機能

namespace Platform
{
    struct WindowConfig
    {
        HE::Uint32 _uWidth        = 0;
        HE::Uint32 _uHeight       = 0;
        HE::Uint32 uViewPortCount = 0;
    };

    struct ViewPortConfig
    {
        HE::Uint32 _uWidth  = 0;
        HE::Uint32 _uHeight = 0;
    };

    /// <summary>
    /// ウィンドウのロジック
    /// </summary>
    class WindowStrategy
    {
    public:
        WindowStrategy(const WindowConfig&);

        virtual void VRelease() {}

        virtual void VShow()  = 0;
        virtual void VBegin() = 0;
        virtual void VEnd()   = 0;

        virtual void VBeginRender() = 0;
        virtual void VEndRender()   = 0;

        inline const WindowConfig& GetConfig() const { return this->_config; }

    protected:
        WindowConfig _config;
    };

    /// <summary>
    /// ビューポートのロジック
    /// </summary>
    class ViewPortStrategy
    {
    public:
        ViewPortStrategy(const ViewPortConfig&);

        virtual void VBeginRender() = 0;
        virtual void VEndRender()   = 0;

        inline const ViewPortConfig& GetConfig() const { return this->_config; }

    protected:
        ViewPortConfig _config;
    };

    /// <summary>
    /// 描画シーンのロジックインターフェイス
    /// </summary>
    class SceneStrategyInterface
    {
    public:
        virtual ~SceneStrategyInterface() = default;

        virtual HE::Bool VBegin() = 0;
        virtual void VEnd()       = 0;

        virtual void VUpdate(const HE::Float32) = 0;
        virtual void VBeginRender()             = 0;
        virtual void VEndRender()               = 0;
    };

    class ScreenInterface
    {
    public:
        virtual ~ScreenInterface() = default;
        virtual void VRelease()    = 0;

        virtual Core::Memory::UniquePtr<WindowStrategy> VCreateWindowStrategy(
            const WindowConfig&) = 0;

        virtual Core::Memory::UniquePtr<ViewPortStrategy> VCreateViewPortStrategy(
            const ViewPortConfig&) = 0;

        virtual Core::Memory::UniquePtr<SceneStrategyInterface> VCreateSceneUIStrategy() = 0;
        virtual Core::Memory::UniquePtr<SceneStrategyInterface> VCreateScene2DStrategy() = 0;

        /// <summary>
        /// 画面を色クリア
        /// </summary>
        virtual void VCls(const HE::Uint32 in_uR, const HE::Uint32 in_uG,
                          const HE::Uint32 in_uB) = 0;

        /// <summary>
        /// 2Dのテキスト描画
        /// </summary>
        virtual void VDrawText2D(const ViewPortConfig& in_rViewConfg,
                                 const Core::Math::Vector2& in_rPos,
                                 const Core::Math::EAnchor in_eAnchor, const HE::Char* in_szText,
                                 const HE::Uint32 in_uTextSize, const Core::Math::Color) = 0;

        /// <summary>
        /// 2Dの矩形描画
        /// </summary>
        virtual void VDrawQuad2D(const ViewPortConfig& in_rViewConfig,
                                 const Core::Math::Rect2& in_rRect2D, const Core::Math::Color) = 0;

        /// <summary>
        /// 2Dの円描画
        /// </summary>
        virtual void VDrawCircle2D(const ViewPortConfig& in_rViewConfig,
                                   const Core::Math::Vector2& in_rPos,
                                   const Core::Math::EAnchor in_eAchor, const HE::Float32 in_fSize,
                                   const Core::Math::Color) = 0;
    };
}  // namespace Platform
