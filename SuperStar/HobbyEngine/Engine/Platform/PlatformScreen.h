#pragma once

#include <functional>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/Handle.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"

// プラットフォームの描画に関わる機能

namespace Platform
{
    /// <summary>
    /// ウィンドウの設定データ
    /// </summary>
    class WindowConfig
    {
    public:
        enum EFlags
        {
            EFlags_None            = 0x00000000,
            EFlags_Resizable       = 0x00000010,
            EFlags_Minimized       = 0x00000020,
            EFlags_Maximized       = 0x00000040,
            EFlags_WinDisableCloseBtn = 0x00000080,
        };

        /// <summary>
        /// ウィンドウのメニュー項目について
        /// メニュー項目の入れ子項目が今は作れない
        /// </summary>
        struct WindowMenuItem
        {
            HE::Char szName[128] = {};
        };

        using MenuItemMap = Core::Common::FixedMap<HE::Uint32, WindowMenuItem, 32>;

    public:
        WindowConfig(const HE::Uint32 in_uW, const HE::Uint32 in_uH,
                     const HE::Uint32 in_uViewPortCount, const HE::Bool in_bMain,
                     const Core::Common::Handle in_inputHandle,
                     const EFlags in_eFlags = EFlags_None);
        WindowConfig(const WindowConfig& in_rOrg);

        inline const HE::Uint32 Width() const { return this->_uWidth; }
        inline const HE::Uint32 Height() const { return this->_uHeight; }
        inline const HE::Uint32 ViewPortCount() const { return this->_uViewPortCount; }
        inline const Core::Common::Handle InputHandle() const { return this->_inputHandle; }
        inline const HE::Bool IsMain() const { return this->_bMain; }
        inline const EFlags Flags() const { return this->_eFlags; }

        const MenuItemMap& GetMenuItemMap() const { return this->_mMenuItem; }

        /// <summary>
        /// TODO: ウィンドウにメニューアイテムを追加
        /// </summary>
        const HE::Bool AddMenuItem(const HE::Uint32 in_uID, const HE::Char* in_szName);

    private:
        EFlags _eFlags             = EFlags_None;
        HE::Uint32 _uWidth         = 0;
        HE::Uint32 _uHeight        = 0;
        HE::Uint32 _uViewPortCount = 0;
        HE::Bool _bMain            = FALSE;
        Core::Common::Handle _inputHandle;

        MenuItemMap _mMenuItem;
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
        /// <summary>
        /// メニューイベントのコールバック型
        /// </summary>
        using EventMenuCallback = std::function<void(HE::Uint32)>;

    public:
        WindowStrategy(const Core::Common::Handle, const WindowConfig&) {}
        virtual void VRelease() = 0;

        virtual void VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY) = 0;

        virtual void VActive() = 0;
        virtual void VShow()   = 0;
        virtual void VHide()   = 0;

        virtual void VBegin()                         = 0;
        virtual void VEnd()                           = 0;
        virtual void VUpdate(const HE::Float32 in_dt) = 0;

        virtual void VBeginRender() = 0;
        virtual void VEndRender()   = 0;

        virtual const HE::Bool VIsClose() const = 0;

#ifdef HE_USE_SDL2
        virtual void* VGetWindowBySDL2() const { return NULL; }
        virtual void* VGetContentBySDL2() const { return NULL; }
#endif
        virtual void VRegistEventMenuCallback(EventMenuCallback in_callback) = 0;

        virtual const Core::Common::Handle VGetHandle() const = 0;
        virtual const WindowConfig& VGetConfig() const        = 0;
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
    /// シーンのロジックインターフェイス
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

        /// <summary>
        /// シーンを描画するプラットフォームのインスタンス
        /// </summary>
        /// <returns></returns>
        virtual class ScreenRenderInterface* VGetPlatformScreenDraw() { return NULL; }
    };

    /// <summary>
    /// シーンレンダリングのインターフェイス
    /// </summary>
    class ScreenRenderInterface
    {
    public:
        /// <summary>
        /// 画面を色クリア
        /// </summary>
        virtual void VCls(const HE::Uint32 in_uR, const HE::Uint32 in_uG,
                          const HE::Uint32 in_uB) = 0;

        // TODO: パーティクルの描画
        virtual void V2DDrawPartical(const Platform::ViewPortConfig& in_rViewConfig,
                                     const Core::Common::Handle in_rParticleHandle,
                                     const Core::Math::Vector3&) = 0;

        /// <summary>
        /// 2Dのテキスト描画
        /// </summary>
        virtual void V2DDrawText(const ViewPortConfig&, const Core::Math::Vector2& in_rPos,
                                 const Core::Math::EAnchor in_eAnchor, const HE::Char* in_szText,
                                 const HE::Uint32 in_uTextSize, const HE::Uint32 in_uSpace,
                                 const Core::Math::Color) = 0;

        /// <summary>
        /// 2Dの矩形描画
        /// </summary>
        virtual void V2DDrawQuad(const ViewPortConfig& in_rViewConfig,
                                 const Core::Math::Rect2& in_rRect2D, const Core::Math::Color) = 0;

        /// <summary>
        /// 2Dの円描画
        /// </summary>
        virtual void V2DDrawCircle(const ViewPortConfig& in_rViewConfig,
                                   const Core::Math::Vector2& in_rPos,
                                   const Core::Math::EAnchor in_eAchor, const HE::Float32 in_fSize,
                                   const Core::Math::Color) = 0;

        /// <summary>
        /// 2Dの三角形描画
        /// </summary>
        virtual void V2DDrawTriangle(const ViewPortConfig& in_rViewConfig,
                                     const Core::Math::Vector2& in_rPos,
                                     const Core::Math::EAnchor in_eAchor,
                                     const HE::Float32 in_rAngleDegress, const HE::Float32 in_fSize,
                                     const Core::Math::Color) = 0;
    };

    class ScreenInterface
    {
    public:
        virtual ~ScreenInterface() = default;
        virtual void VRelease()    = 0;

        virtual Core::Memory::UniquePtr<WindowStrategy> VCreateWindowStrategy(
            const Core::Common::Handle, const WindowConfig&) = 0;

        virtual Core::Memory::UniquePtr<ViewPortStrategy> VCreateViewPortStrategy(
            const ViewPortConfig&) = 0;

        virtual Core::Memory::UniquePtr<SceneStrategyInterface> VCreateSceneUIStrategy() = 0;
        virtual Core::Memory::UniquePtr<SceneStrategyInterface> VCreateScene2DStrategy() = 0;

        // TODO: パーティクルの生成
        // 描画は戻り値のハンドルを指定
        // 事前に生成しておく必要がある
        virtual Core::Common::Handle VParticalCreate(const HE::Uint32 in_uCount) = 0;

        /// <summary>
        /// TODO: 生成したパーティクルを削除
        /// </summary>
        virtual void VParticalDelete(Core::Common::Handle) = 0;

        virtual void VParticalSetPositions(const Core::Common::Handle,
                                           const Core::Common::ArrayBase<Core::Math::Vector3>&) = 0;
        virtual void VParticalSetVelocitys(const Core::Common::Handle,
                                           const Core::Common::ArrayBase<Core::Math::Vector3>&) = 0;

        virtual ScreenRenderInterface* VGetDrawInterface() = 0;
    };

}  // namespace Platform
