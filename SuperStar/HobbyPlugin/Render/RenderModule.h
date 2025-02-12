#pragma once

#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Render/Command/Command.h"
#include "Render/Particle/Blob.h"
#include "Render/Window/Scene.h"
#include "Render/Window/ViewPort.h"
#include "Render/Window/Window.h"

namespace Render
{
    /// <summary>
    /// レンダリングのコンテキスト
    /// </summary>
    struct RenderingContext
    {
    public:
        void Setup(const Core::Common::Handle& in_rWindowHandle,
                   const Core::Common::Handle& in_rViewPortHandle,
                   const Core::Common::Handle& in_rSceneHandle);

        inline Core::Common::Handle GetWindowHandle() const { return this->ulWindowHandle; }
        inline Core::Common::Handle GetViewPortHandle() const { return this->ulViewPortHandle; }
        inline Core::Common::Handle GetSceneHandle() const { return this->ulSceneHandle; }

        // コンテキストが有効かどうか
        HE::Bool IsValid() const;

    private:
        Core::Common::Handle ulWindowHandle;
        Core::Common::Handle ulViewPortHandle;
        Core::Common::Handle ulSceneHandle;
    };

    /// <summary>
    /// 描画モジュール
    /// </summary>
    class RenderModule final : public Module::ModuleBase
    {
        HE_CLASS_COPY_NG(RenderModule);
        HE_CLASS_MOVE_NG(RenderModule);
        HE_MODULE_GENRATE_DECLARATION(RenderModule);

    public:
        using ViewHandleVector   = Core::Common::FixedVector<Core::Common::Handle, 32>;
        using WindowHandleKeyMap = Core::Common::FixedMap<HE::Uint64, Core::Common::Handle, 32>;
        using ParticleBlobObject = std::tuple<Core::Common::Handle, Render::Prticle::Blob*>;

        using OnCallbackBeginWindow = std::function<void(Core::Common::Handle)>;
        using OnCallbackEndWindow   = std::function<void(Core::Common::Handle)>;

    public:
        RenderModule();

        /// <summary>
        /// ウィンドウ生成
        /// </summary>
        const Core::Common::Handle NewWindow(const HE::Uint32 in_w, const HE::Uint32 in_h);

        /// <summary>
        /// ウィンドウ破棄
        /// </summary>
        HE::Bool DeleteWindow(Core::Common::Handle&);
        void DeleteAllWindow();

        /// <summary>
        /// ウィンドウ表示
        /// </summary>
        void ShowWindow(const Core::Common::Handle);

        /// <summary>
        /// ウィンドウ取得
        /// </summary>
        Window* GetWindow(const Core::Common::Handle);

        /// <summary>
        /// ビューポート追加
        /// </summary>
        const Core::Common::Handle AddViewPort(const Core::Common::Handle, const HE::Uint32 in_w,
                                               const HE::Uint32 in_h);

        /// <summary>
        /// ビューポート外す
        /// </summary>
        HE::Bool RemoveViewPort(const Core::Common::Handle in_rWindowHandle,
                                Core::Common::Handle& in_rViewPortHandle);

        const ViewPort* GetViewPort(const Core::Common::Handle);

        /// <summary>
        /// UI用シーン追加
        /// </summary>
        std::tuple<Core::Common::Handle, SceneViewBase*> AddSceneViewUI(
            const Core::Common::Handle in_rWindowsHandle,
            const Core::Common::Handle in_rViewPortHash);

        /// <summary>
        /// 2D用シーン追加
        /// </summary>
        std::tuple<Core::Common::Handle, SceneViewBase*> AddSceneView2D(
            const Core::Common::Handle in_rWindowsHandle,
            const Core::Common::Handle in_rViewPortHash);

        /// <summary>
        /// TODO: パーティクルの塊オブジェクト生成
        /// </summary>
        ParticleBlobObject CreatePrticle(const Core::Common::Handle);

        /// <summary>
        /// TODO: 生成したパーティクルを削除
        /// </summary>
        void DeletePrticle(Core::Common::Handle&);

        /// <summary>
        /// 生成したパーティクルを取得
        /// </summary>
        Render::Prticle::Blob& GetPrticle(const Core::Common::Handle);

        /// <summary>
        /// レンダリングするコマンド追加
        /// </summary>
        HE::Bool PushRenderCommand(const Core::Common::Handle, Command&&);

        // ウィンドウイベント関連
        // TODO: 追加という名前になっているが, 現在は一つしか追加できない
        // 後々追加に変更対応
        void AddEventBeginWindow(OnCallbackBeginWindow);
        void AddEventEndWindow(OnCallbackEndWindow);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        HE::Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;

        /// <summary>
        /// 前更新
        /// </summary>
        void _VBeforeUpdate(const HE::Float32) override final;

        /// <summary>
        /// モジュール更新
        /// </summary>
        void _VUpdate(const HE::Float32) override final;

        /// <summary>
        /// モジュール後更新
        /// </summary>
        void _VLateUpdate(const HE::Float32) override final;

    private:
        SceneViewBase* _GetSceneBase(const Core::Common::Handle);

        Core::Common::Handle _AddScene(const Core::Common::Handle& in_rWindowHandle,
                                       const Core::Common::Handle& in_rViewPortHandle,
                                       const Core::Common::Handle& in_rSceneHandle);

    private:
        Core::Common::FixedPoolManager<Window, 32> _poolWindow;
        Core::Common::FixedPoolManager<RenderingContext, 32> _poolRenderingContext;

        Core::Common::FixedStack<Core::Common::Handle, 32> _sStandupWindow;
        // TODO: 自前のを作ったら差し替える
        std::unordered_map<Core::Common::Handle, Render::Prticle::Blob*> _mParticleHandle;

        OnCallbackBeginWindow _onBeginWindow = NULL;
        OnCallbackEndWindow _onEndWindow     = NULL;
    };

}  // namespace Render
