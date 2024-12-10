#include "RenderModule.h"

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

    const Core::Common::Handle RenderModule::NewWindow(
        Core::Memory::UniquePtr<Render::WindowStrategy> in_upStrategy)
    {
        auto [handle, pWindow] = this->_poolWindow.Alloc();
        if (handle.Null()) return NullHandle;

        // 作成したウィンドウのセットアップ
        if (pWindow->_Setup(std::move(in_upStrategy)) == FALSE)
        {
            this->_poolWindow.Free(handle);
            return NullHandle;
        }
        this->_sStandupWindow.PushBack(handle);

        return handle;
    }

    HE::Bool RenderModule::DeleteWindow(const Core::Common::Handle& in_rHandle)
    {
        if (in_rHandle.Null()) return FALSE;
        // すでに破棄されている
        if (this->_poolWindow.Empty()) return FALSE;

        // Viewの解放処理をする
        auto pWindow = this->_poolWindow.Ref(in_rHandle);
        HE_ASSERT(pWindow &&
                  "ウィンドウがないということは別の箇所でRemoveされいるので意図していない");
        pWindow->_End();

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
        const Core::Common::Handle& in_rWindowHandle,
        Core::Memory::UniquePtr<ViewPortConfig> in_upConfig)
    {
        auto pWindow = this->_GetWindow(in_rWindowHandle);

        return pWindow->AddViewPort(std::move(in_upConfig));
    }

    HE::Bool RenderModule::RemoveViewPort(const Core::Common::Handle& in_rWindowHandle,
                                          const Core::Common::Handle& in_rViewPortHandle)
    {
        auto pWindow = this->_GetWindow(in_rWindowHandle);
        return pWindow->RemoveViewPort(in_rViewPortHandle);
    }

    const ViewPortConfig* RenderModule::GetViewPortConfig(const Core::Common::Handle& in_rHandle)
    {
        auto pRenderingContext = this->_poolRenderingContext.Ref(in_rHandle);

        auto pWindow = this->_GetWindow(pRenderingContext->GetWindowHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pWindow);

        auto pViewPort = pWindow->_Ref(pRenderingContext->GetViewPortHandle());
        HE_ASSERT_RETURN_VALUE(NULL, pViewPort);

        return pViewPort->GetConfig();
    }

    Window* RenderModule::_GetWindow(const Core::Common::Handle& in_rHandle)
    {
        auto pWindow = this->_poolWindow.Ref(in_rHandle);
        HE_ASSERT_RETURN_VALUE(NULL, pWindow);

        return pWindow;
    }

    const Core::Common::Handle& RenderModule::_AddScene(
        const Core::Common::Handle& in_rWindowHandle,
        const Core::Common::Handle& in_rViewPortHandle, const Core::Common::Handle& in_rSceneHandle)
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

        auto pViewPort = pWindow->_Ref(pRenderingContext->GetViewPortHandle());
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

    void RenderModule::_VUpdate(const HE::Float32 in_fDeltaTime)
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
