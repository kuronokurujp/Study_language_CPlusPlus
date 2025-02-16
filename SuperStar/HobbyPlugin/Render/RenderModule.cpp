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

    const Core::Common::Handle RenderModule::NewWindow(
        OnFactoryWindowStrategyCallback in_onFactoryStrategyCallback)
    {
        auto [handle, pWindow] = this->_poolWindow.Alloc();
        if (handle.Null()) return NullHandle;

        auto upStrategy = in_onFactoryStrategyCallback(handle);
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
            this->DeleteWindow(itr->_key);
        }
    }

    void RenderModule::ShowWindow(const Core::Common::Handle in_rHandle)
    {
        auto pWindow = this->GetWindow(in_rHandle);
        pWindow->Show();
    }

    Window* RenderModule::GetWindow(const Core::Common::Handle in_rHandle)
    {
        auto pWindow = this->_poolWindow.Ref(in_rHandle);
        HE_ASSERT_RETURN_VALUE(NULL, pWindow);

        return pWindow;
    }

    const Core::Common::Handle RenderModule::AddViewPort(
        const Core::Common::Handle in_rWindowHandle, const HE::Uint32 in_w, const HE::Uint32 in_h)
    {
        auto pWindow = this->GetWindow(in_rWindowHandle);

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

    HE::Bool RenderModule::RemoveViewPort(const Core::Common::Handle in_rWindowHandle,
                                          Core::Common::Handle& in_rViewPortHandle)
    {
        auto pWindow = this->GetWindow(in_rWindowHandle);
        return pWindow->RemoveViewPort(in_rViewPortHandle);
    }

    const ViewPort* RenderModule::GetViewPort(const Core::Common::Handle in_rHandle)
    {
        auto pRenderingContext = this->_poolRenderingContext.Ref(in_rHandle);

        auto pWindow = this->GetWindow(pRenderingContext->GetWindowHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pWindow);

        auto pViewPort = pWindow->_poolViewPortManager.Ref(pRenderingContext->GetViewPortHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pViewPort);

        return pViewPort;
    }

    std::tuple<Core::Common::Handle, SceneViewBase*> RenderModule::AddSceneView(
        const Core::Common::Handle in_rWindowsHandle, const Core::Common::Handle in_rViewPortHash,
        OnFactorySceneViewStrategyCallback in_callback)
    {
        auto pWindow   = this->GetWindow(in_rWindowsHandle);
        auto pViewPort = pWindow->_poolViewPortManager.Ref(in_rViewPortHash);

        // コールバックからシーンのストラテジーを取得して渡す
        // 利用者側が任意でシーン内容を選択できるようにするため
        auto upStrategy            = in_callback();
        auto [sceneHandle, pScene] = pViewPort->AddSceneView(std::move(upStrategy));

        Core::Common::Handle handle =
            this->_AddScene(in_rWindowsHandle, in_rViewPortHash, sceneHandle);
        if (handle == NullHandle)
        {
            pViewPort->RemoveScene(sceneHandle);
        }

        return std::tuple<Core::Common::Handle, SceneViewBase*>(handle, pScene);
    }

    RenderModule::ParticleBlobObject RenderModule::CreatePrticle(
        const Core::Common::Handle in_renderHandle)
    {
        HE_ASSERT_RETURN_VALUE((ParticleBlobObject(NullHandle, NULL)),
                               in_renderHandle.Null() == FALSE);

        auto pScene = this->_GetSceneBase(in_renderHandle);
        HE_ASSERT_RETURN_VALUE((ParticleBlobObject(NullHandle, NULL)),
                               in_renderHandle.Null() == FALSE);

        // TODO: パーティクル描画ハンドルを生成
        auto upCreateParticlaHandler =
            HE_MAKE_CUSTOM_UNIQUE_PTR((Render::Prticle::Blob::CreateFunctionType),
                                      [this](HE::Uint32 in_uCount)
                                      {
                                          auto pPlatform =
                                              this->GetDependenceModule<Platform::PlatformModule>();
                                          return pPlatform->VScreen()->VParticalCreate(in_uCount);
                                      });

        auto upDeleteParticleHandler =
            HE_MAKE_CUSTOM_UNIQUE_PTR((Render::Prticle::Blob::DeleteFunctionType),
                                      [this](Core::Common::Handle in_rHandle)
                                      {
                                          auto pPlatform =
                                              this->GetDependenceModule<Platform::PlatformModule>();
                                          pPlatform->VScreen()->VParticalDelete(in_rHandle);
                                      });

        auto upPositionHandler = HE_MAKE_CUSTOM_UNIQUE_PTR(
            (Render::Prticle::Blob::PositionFunctionType),
            [this](const Core::Common::Handle in_handle,
                   const Core::Common::ArrayBase<Core::Math::Vector3>& in_arPosition)
            {
                auto pPlatform = this->GetDependenceModule<Platform::PlatformModule>();
                pPlatform->VScreen()->VParticalSetPositions(in_handle, in_arPosition);
            });

        auto upVelocityHandler = HE_MAKE_CUSTOM_UNIQUE_PTR(
            (Render::Prticle::Blob::VelocityFunctionType),
            [this](const Core::Common::Handle in_handle,
                   const Core::Common::ArrayBase<Core::Math::Vector3>& in_arVelocity)
            {
                auto pPlatform = this->GetDependenceModule<Platform::PlatformModule>();
                pPlatform->VScreen()->VParticalSetVelocitys(in_handle, in_arVelocity);
            });

        auto [handle, pBlob] = pScene->_runtimePoolPrticleBlob.Alloc<Render::Prticle::Blob>(
            in_renderHandle, std::move(upCreateParticlaHandler), std::move(upDeleteParticleHandler),
            std::move(upPositionHandler), std::move(upVelocityHandler));
        this->_mParticleHandle[handle] = pBlob;

        return ParticleBlobObject(handle, pBlob);
    }

    void RenderModule::DeletePrticle(Core::Common::Handle& in_rPrticleHandle)
    {
        HE_ASSERT_RETURN(in_rPrticleHandle.Null() == FALSE);
        if (this->_mParticleHandle.empty()) return;
        if (this->_mParticleHandle.find(in_rPrticleHandle) == this->_mParticleHandle.end()) return;

        auto pBlob = this->_mParticleHandle[in_rPrticleHandle];

        auto pScene = this->_GetSceneBase(pBlob->GetRenderHandle());
        HE_ASSERT_RETURN(pScene);

        // TODO: 管理から外す
        this->_mParticleHandle.erase(in_rPrticleHandle);

        pBlob->Release();
        pScene->_runtimePoolPrticleBlob.Free(in_rPrticleHandle, FALSE);
    }

    Render::Prticle::Blob& RenderModule::GetPrticle(const Core::Common::Handle in_prticleHandle)
    {
        HE_ASSERT(in_prticleHandle.Null() == FALSE);

        auto pBlob = this->_mParticleHandle[in_prticleHandle];

        auto pScene = this->_GetSceneBase(pBlob->GetRenderHandle());
        HE_ASSERT(pScene);

        return *(pScene->_runtimePoolPrticleBlob.Ref(in_prticleHandle));
    }

    SceneViewBase* RenderModule::_GetSceneBase(const Core::Common::Handle in_renderHandle)
    {
        auto pRenderingContext = this->_poolRenderingContext.Ref(in_renderHandle);

        auto pWindow = this->GetWindow(pRenderingContext->GetWindowHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pWindow);

        auto pViewPort = pWindow->_poolViewPortManager.Ref(pRenderingContext->GetViewPortHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pViewPort);

        auto pScene = pViewPort->GetScene(pRenderingContext->GetSceneHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pScene);

        return pScene;
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

    HE::Bool RenderModule::PushRenderCommand(const Core::Common::Handle in_renderHandle,
                                             Command&& in_rrCommand)
    {
        auto pRenderingContext = this->_poolRenderingContext.Ref(in_renderHandle);

        auto pWindow = this->GetWindow(pRenderingContext->GetWindowHandle());
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
        // パーティクルを全削除
        auto itr = this->_mParticleHandle.begin();
        while (itr != this->_mParticleHandle.end())
        {
            auto handle = itr->first;
            this->DeletePrticle(handle);

            itr = this->_mParticleHandle.begin();
        }

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

    void RenderModule::_VUpdate(const HE::Float32 in_fDeltaTime)
    {
        // ウィンドウを更新
        auto mWindow = this->_poolWindow.GetUseDataMap();
        for (auto itr = mWindow.Begin(); itr != mWindow.End(); ++itr)
        {
            itr->_data->_Update(in_fDeltaTime);
        }
    }

    void RenderModule::_VLateUpdate(const HE::Float32)
    {
        // ウィンドウの描画
        auto mWindow = this->_poolWindow.GetUseDataMap();
        for (auto itr = mWindow.Begin(); itr != mWindow.End(); ++itr)
        {
            itr->_data->_Render();
        }
    }

}  // namespace Render
