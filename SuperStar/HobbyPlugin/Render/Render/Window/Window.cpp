#include "Window.h"

#include "./../Particle/Blob.h"
#include "./Scene.h"
#include "./ViewPort.h"
#include "Engine/Engine.h"
#include "Engine/Platform/PlatformModule.h"

namespace Render
{
    const Platform::WindowConfig* Window::GetConfig()
    {
        return &this->_upStrategy->VGetConfig();
    }

    void Window::RegistEventMenuCallback(Platform::WindowStrategy::EventMenuCallback callback)
    {
        this->_upStrategy->VRegistEventMenuCallback(callback);
    }

    void Window::SetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY)
    {
        this->_uX = in_uX;
        this->_uY = in_uY;

        if (this->_bReady) this->_upStrategy->VSetPos(this->_uX, this->_uY);
    }

    Core::Common::Handle Window::AddViewPort(
        Core::Memory::UniquePtr<Platform::ViewPortStrategy> in_upStg)
    {
        auto [handle, pViewPort] = this->_poolViewPortManager.Alloc<ViewPort>();

        // TODO: 用意できるシーン数は固定にしておく
        pViewPort->Init(std::move(in_upStg), 3);

        return handle;
    }

    HE::Bool Window::RemoveViewPort(Core::Common::Handle& in_rHandle)
    {
        auto pViewPort = this->_poolViewPortManager.Ref(in_rHandle);
        HE_ASSERT(pViewPort);

        pViewPort->_End();
        pViewPort->Release();

        this->_poolViewPortManager.Free(in_rHandle, FALSE);

        return TRUE;
    }

    void Window::Show()
    {
        if (this->_bReady) this->_upStrategy->VShow();
        this->_bShow = TRUE;
    }

    void Window::Hide()
    {
        if (this->_bReady) this->_upStrategy->VHide();
        this->_bShow = FALSE;
    }

    const HE::Bool Window::IsClose() const
    {
        return this->_upStrategy->VIsClose();
    }

    HE::Bool Window::Init(Core::Memory::UniquePtr<Platform::WindowStrategy> in_upConfig)
    {
        this->_upStrategy = std::move(in_upConfig);

        auto rWindowConfig = this->_upStrategy->VGetConfig();
        HE_ASSERT(0 < rWindowConfig.ViewPortCount());

        this->_poolViewPortManager.ReleasePool();
        this->_poolViewPortManager.POOL_RESERVE_POOL(rWindowConfig.ViewPortCount());

        return TRUE;
    }

    void Window::Release()
    {
        // TODO: ウィンドウが持っている入力オブジェクトを削除
        auto inputHandle = this->_upStrategy->VGetConfig().InputHandle();
        HE_ENGINE.PlatformModule()->VInput()->VReleaseObject(inputHandle);

        this->_poolViewPortManager.ReleasePool([](ViewPort* in_pViewPort)
                                               { in_pViewPort->Release(); });

        this->_upStrategy->VRelease();
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upStrategy);
    }

    void Window::_Begin()
    {
        this->_upStrategy->VBegin();

        this->_upStrategy->VActive();

        // ウィンドウ座標値があれば設定する
        if (this->_uX != HE::uInvalidUint32 && this->_uY != HE::uInvalidUint32)
            this->_upStrategy->VSetPos(this->_uX, this->_uY);

        if (this->_bShow)
        {
            this->_upStrategy->VShow();
        }
        this->_bReady = TRUE;
    }

    void Window::_End()
    {
        this->_upStrategy->VActive();

        this->_upStrategy->VEnd();

        {
            auto m = this->_poolViewPortManager.GetUserDataList();
            HE_ASSERT_RETURN(m);

            for (auto itr = m->begin(); itr != m->end(); ++itr)
            {
                itr->second->_End();
            }
        }
    }

    void Window::_Update(const HE::Float32 in_fDt)
    {
        this->_upStrategy->VActive();
        this->_upStrategy->VUpdate(in_fDt);

        // ビューポート処理
        auto m = this->_poolViewPortManager.GetUserDataList();
        if (m)
        {
            for (auto itrViewPort = m->begin(); itrViewPort != m->end(); ++itrViewPort)
            {
                auto pViewPort = itrViewPort->second;

                // シーン更新
                // シーンの実際の更新はストラテジークラス側で行う
                auto m = pViewPort->_poolSceneManager.GetUserDataList();
                if (m == NULL) continue;

                for (auto itrScene = m->begin(); itrScene != m->end(); ++itrScene)
                {
                    itrScene->second->_Update(in_fDt);
                }
            }
        }
    }

    void Window::_Render()
    {
        this->_upStrategy->VActive();

        this->_upStrategy->VBeginRender();

        // ビューポート処理
        auto m = this->_poolViewPortManager.GetUserDataList();
        if (m)
        {
            for (auto itrViewPort = m->begin(); itrViewPort != m->end(); ++itrViewPort)
            {
                // シーン描画
                auto pViewPort = itrViewPort->second;
                auto m         = pViewPort->_poolSceneManager.GetUserDataList();
                if (m == NULL) continue;

                pViewPort->_BeginRender();

                Platform::ViewPortConfig viewPortConfig;
                {
                    viewPortConfig._uWidth  = pViewPort->Width();
                    viewPortConfig._uHeight = pViewPort->Height();
                }

                for (auto itrScene = m->begin(); itrScene != m->end(); ++itrScene)
                {
                    itrScene->second->_BeginRender();

                    // ビュー毎に描画コマンド処理
                    itrScene->second->_Render(viewPortConfig);

                    itrScene->second->_EndRender();
                }

                pViewPort->_EndRender();
            }
        }

        this->_upStrategy->VEndRender();
    }

}  // namespace Render
