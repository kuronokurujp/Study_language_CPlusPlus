#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/Common/Hash.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Render/Color.h"
#include "Render/Command/Command.h"
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

    public:
        RenderModule() : ModuleBase(ModuleName(), Module::ELayer_View, 10) {}

        /// <summary>
        /// ウィンドウ生成
        /// </summary>
        const Core::Common::Handle NewWindow(Core::Memory::UniquePtr<Render::WindowStrategy>);

        /// <summary>
        /// ウィンドウ破棄
        /// </summary>
        HE::Bool DeleteWindow(const Core::Common::Handle&);
        void DeleteAllWindow();

        /// <summary>
        /// ウィンドウ表示
        /// </summary>
        void ShowWindow(const Core::Common::Handle&);

        /// <summary>
        /// ビューポート追加
        /// </summary>
        const Core::Common::Handle AddViewPort(const Core::Common::Handle&,
                                               Core::Memory::UniquePtr<ViewPortConfig>);
        /// <summary>
        /// ビューポート外す
        /// </summary>
        HE::Bool RemoveViewPort(const Core::Common::Handle& in_rWindowHandle,
                                const Core::Common::Handle& in_rViewPortHandle);

        const ViewPortConfig* GetViewPortConfig(const Core::Common::Handle&);

        /// <summary>
        /// UI用シーン追加
        /// </summary>
        template <typename TScene>
        const Core::Common::Handle& AddSceneViewUI(const Core::Common::Handle& in_rWindowsHandle,
                                                   const Core::Common::Handle& in_rViewPortHash)
        {
            auto pWindow   = this->_GetWindow(in_rWindowsHandle);
            auto pViewPort = pWindow->_Ref(in_rViewPortHash);

            auto sceneHandle = pViewPort->AddSceneViewUI<TScene>();
            const Core::Common::Handle& handle =
                this->_AddScene(in_rWindowsHandle, in_rViewPortHash, sceneHandle);
            if (handle == NullHandle)
            {
                pViewPort->RemoveScene(sceneHandle);
            }

            return handle;
        }

        /// <summary>
        /// 2D用シーン追加
        /// </summary>
        template <typename TScene>
        const Core::Common::Handle& AddSceneView2D(const Core::Common::Handle& in_rWindowsHandle,
                                                   const Core::Common::Handle& in_rViewPortHash)

        {
            auto pWindow   = this->_GetWindow(in_rWindowsHandle);
            auto pViewPort = pWindow->_Ref(in_rViewPortHash);

            auto sceneHandle = pViewPort->AddSceneView2D<TScene>();

            const Core::Common::Handle& handle =
                this->_AddScene(in_rWindowsHandle, in_rViewPortHash, sceneHandle);
            if (handle == NullHandle)
            {
                pViewPort->RemoveScene(sceneHandle);
                return NullHandle;
            }

            return handle;
        }

        /// <summary>
        /// シーンにレンダリングするコマンド追加
        /// </summary>
        HE::Bool PushSceneRenderCommand(const Core::Common::Handle&, Command&&);

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
        HE::Bool _VBeforeUpdate(const HE::Float32) override final;

        /// <summary>
        /// モジュール更新
        /// </summary>
        HE::Bool _VUpdate(const HE::Float32) override final;

        /// <summary>
        /// モジュール後更新
        /// </summary>
        HE::Bool _VLateUpdate(const HE::Float32) override final;

    private:
        Window* _GetWindow(const Core::Common::Handle&);

        const Core::Common::Handle& _AddScene(const Core::Common::Handle& in_rWindowHandle,
                                              const Core::Common::Handle& in_rViewPortHandle,
                                              const Core::Common::Handle& in_rSceneHandle);

    private:
        Core::Common::FixedPoolManager<Window, 32> _poolWindow;
        Core::Common::FixedPoolManager<RenderingContext, 32> _poolRenderingContext;

        Core::Common::FixedStack<Core::Common::Handle, 32> _sStandupWindow;
    };

}  // namespace Render
