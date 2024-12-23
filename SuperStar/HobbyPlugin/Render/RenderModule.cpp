#include "RenderModule.h"

#include "Engine/Platform/PlatformModule.h"

namespace Render
{

    void RenderingContext::Setup(const Core::Common::Handle& in_ulWindowHash,
                                 const Core::Common::Handle& in_ulViewPortHash,
                                 const Core::Common::Handle& in_ulSceneHash)

    {
        this->ulWindowHandle   = in_ulWindowHash;
        this->ulViewPortHandle = in_ulViewPortHash;
        this->ulSceneHandle    = in_ulSceneHash;
    }

    HE::Bool RenderingContext::IsValid() const
    {
        if (this->ulSceneHandle.Null()) return FALSE;
        if (this->ulViewPortHandle.Null()) return FALSE;
        if (this->ulWindowHandle.Null()) return FALSE;

        return TRUE;
    }

    RenderModule::RenderModule() : ModuleBase(ModuleName(), Module::ELayer_View, 10)
    {
        // 依存モジュール
        this->_AppendDependenceModule<Platform::PlatformModule>();
    }

    const Core::Common::Handle RenderModule::NewWindow(const HE::Uint32 in_w, const HE::Uint32 in_h)
    {
        auto [handle, pWindow] = this->_poolWindow.Alloc();
        if (handle.Null()) return NullHandle;

        // TODO: プラットフォームからウィンドウストラテジーを生成して渡す
        // 作成したウィンドウのセットアップ

        Platform::WindowConfig platformWindowConfig;
        {
            platformWindowConfig._uWidth        = in_w;
            platformWindowConfig._uHeight       = in_h;
            platformWindowConfig.uViewPortCount = 1;
        }

        auto pPlatform  = this->GetDependenceModule<Platform::PlatformModule>();
        auto upStrategy = pPlatform->VScreen()->VCreateWindowStrategy(platformWindowConfig);

        if (pWindow->Init(std::move(upStrategy)) == FALSE)
        {
            this->_poolWindow.Free(handle);
            return NullHandle;
        }

        this->_sStandupWindow.PushBack(handle);

        return handle;
    }

    HE::Bool RenderModule::DeleteWindow(Core::Common::Handle& in_rHandle)
    {
        if (in_rHandle.Null()) return FALSE;
        // すでに破棄されている
        if (this->_poolWindow.Empty()) return FALSE;

        // Viewの解放処理をする
        auto pWindow = this->_poolWindow.Ref(in_rHandle);
        HE_ASSERT(pWindow &&
                  "ウィンドウがないということは別の箇所でRemoveされているので意図していない");
        pWindow->_End();
        pWindow->Release();

        // 解放が終わったらプールしているデータを解放
        return this->_poolWindow.Free(in_rHandle);
    }

    void RenderModule::DeleteAllWindow()
    {
        const auto mWindow = this->_poolWindow.GetUseDataMap();

        for (auto itr = mWindow.Begin(); itr != mWindow.End(); ++itr)
        {
            this->DeleteWindow(itr->key);
        }
    }

    void RenderModule::ShowWindow(const Core::Common::Handle& in_rHandle)
    {
        auto pWindow = this->_GetWindow(in_rHandle);
        pWindow->Show();
    }

    const Core::Common::Handle RenderModule::AddViewPort(
        const Core::Common::Handle& in_rWindowHandle, const HE::Uint32 in_w, const HE::Uint32 in_h)
    {
        auto pWindow = this->_GetWindow(in_rWindowHandle);

        // ビューポートの縦横サイズを調整
        // ウィンドウサイズを超えないようにする
        auto rWindowConfig = pWindow->_upStrategy->GetConfig();
        Platform::ViewPortConfig viewPortConfig;
        {
            viewPortConfig._uWidth  = in_w;
            viewPortConfig._uHeight = in_h;
        }

        if (viewPortConfig._uHeight <= 0)
        {
            viewPortConfig._uHeight = rWindowConfig._uHeight;
        }
        else if (rWindowConfig._uHeight < viewPortConfig._uHeight)
        {
            viewPortConfig._uHeight = rWindowConfig._uHeight;
        }

        if (viewPortConfig._uWidth <= 0)
        {
            viewPortConfig._uWidth = rWindowConfig._uWidth;
        }
        else if (rWindowConfig._uWidth < viewPortConfig._uWidth)
        {
            viewPortConfig._uWidth = rWindowConfig._uWidth;
        }

        auto pPlatform  = this->GetDependenceModule<Platform::PlatformModule>();
        auto upStrategy = pPlatform->VScreen()->VCreateViewPortStrategy(viewPortConfig);

        return pWindow->AddViewPort(std::move(upStrategy));
    }

    HE::Bool RenderModule::RemoveViewPort(const Core::Common::Handle& in_rWindowHandle,
                                          Core::Common::Handle& in_rViewPortHandle)
    {
        auto pWindow = this->_GetWindow(in_rWindowHandle);
        return pWindow->RemoveViewPort(in_rViewPortHandle);
    }

    const ViewPort* RenderModule::GetViewPort(const Core::Common::Handle& in_rHandle)
    {
        auto pRenderingContext = this->_poolRenderingContext.Ref(in_rHandle);

        auto pWindow = this->_GetWindow(pRenderingContext->GetWindowHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pWindow);

        auto pViewPort = pWindow->_poolViewPortManager.Ref(pRenderingContext->GetViewPortHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pViewPort);

        return pViewPort;  //->GetConfig();
    }

    std::tuple<Core::Common::Handle, SceneViewBase*> RenderModule::AddSceneViewUI(
        const Core::Common::Handle& in_rWindowsHandle, const Core::Common::Handle& in_rViewPortHash)
    {
        auto pWindow   = this->_GetWindow(in_rWindowsHandle);
        auto pViewPort = pWindow->_poolViewPortManager.Ref(in_rViewPortHash);

        // TODO: プラットフォームからシーンを取得して渡す
        auto pPlatform  = this->GetDependenceModule<Platform::PlatformModule>();
        auto upStrategy = pPlatform->VScreen()->VCreateSceneUIStrategy();

        auto [sceneHandle, pScene] = pViewPort->AddSceneView(std::move(upStrategy));

        Core::Common::Handle handle =
            this->_AddScene(in_rWindowsHandle, in_rViewPortHash, sceneHandle);
        if (handle == NullHandle)
        {
            pViewPort->RemoveScene(sceneHandle);
        }

        return std::tuple<Core::Common::Handle, SceneViewBase*>(handle, pScene);
    }

    std::tuple<Core::Common::Handle, SceneViewBase*> RenderModule::AddSceneView2D(
        const Core::Common::Handle& in_rWindowsHandle, const Core::Common::Handle& in_rViewPortHash)
    {
        auto pWindow   = this->_GetWindow(in_rWindowsHandle);
        auto pViewPort = pWindow->_poolViewPortManager.Ref(in_rViewPortHash);

        auto pPlatform  = this->GetDependenceModule<Platform::PlatformModule>();
        auto upStrategy = pPlatform->VScreen()->VCreateScene2DStrategy();

        auto [sceneHandle, pScene] = pViewPort->AddSceneView(std::move(upStrategy));

        const Core::Common::Handle& handle =
            this->_AddScene(in_rWindowsHandle, in_rViewPortHash, sceneHandle);
        if (handle == NullHandle)
        {
            pViewPort->RemoveScene(sceneHandle);
        }

        return std::tuple<Core::Common::Handle, SceneViewBase*>(handle, pScene);
    }

    Window* RenderModule::_GetWindow(const Core::Common::Handle& in_rHandle)
    {
        auto pWindow = this->_poolWindow.Ref(in_rHandle);
        HE_ASSERT_RETURN_VALUE(NULL, pWindow);

        return pWindow;
    }

    Core::Common::Handle RenderModule::_AddScene(const Core::Common::Handle& in_rWindowHandle,
                                                 const Core::Common::Handle& in_rViewPortHandle,
                                                 const Core::Common::Handle& in_rSceneHandle)
    {
        auto [handle, pRenderingContext] = this->_poolRenderingContext.Alloc();
        pRenderingContext->Setup(in_rWindowHandle, in_rViewPortHandle, in_rSceneHandle);
        if (pRenderingContext->IsValid() == FALSE)
        {
            return NullHandle;
        }

        return handle;
    }

    HE::Bool RenderModule::PushSceneRenderCommand(const Core::Common::Handle& in_rHandle,
                                                  Command&& in_rrCommand)
    {
        auto pRenderingContext = this->_poolRenderingContext.Ref(in_rHandle);

        auto pWindow = this->_GetWindow(pRenderingContext->GetWindowHandle());
        HE_ASSERT(pWindow);

        auto pViewPort = pWindow->_poolViewPortManager.Ref(pRenderingContext->GetViewPortHandle());
        HE_ASSERT(pViewPort);

        auto pScene = pViewPort->GetScene(pRenderingContext->GetSceneHandle());
        HE_ASSERT(pScene);

        return pScene->_PushCommand(std::move(in_rrCommand));
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool RenderModule::_VStart()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool RenderModule::_VRelease()
    {
        // ウィンドウ解放
        this->DeleteAllWindow();

        return TRUE;
    }

    void RenderModule::_VBeforeUpdate(const HE::Float32 in_fDeltaTime)
    {
        // 作成したウィンドウの立ち上げ
        while (this->_sStandupWindow.Empty() == FALSE)
        {
            auto handle  = this->_sStandupWindow.PopBack();
            auto pWindow = this->_poolWindow.Ref(handle);
            if (pWindow)
            {
                pWindow->_Begin();
            }
        }
    }

    void RenderModule::_Update(const HE::Float32 in_fDeltaTime)
    {
        // ウィンドウを更新
        auto mWindow = this->_poolWindow.GetUseDataMap();
        for (auto itr = mWindow.Begin(); itr != mWindow.End(); ++itr)
        {
            itr->data->_Update(in_fDeltaTime);
        }
    }

    void RenderModule::_VLateUpdate(const HE::Float32)
    {
        // ウィンドウの描画
        auto mWindow = this->_poolWindow.GetUseDataMap();
        for (auto itr = mWindow.Begin(); itr != mWindow.End(); ++itr)
        {
            itr->data->_Render();
        }
    }

}  // namespace Render
