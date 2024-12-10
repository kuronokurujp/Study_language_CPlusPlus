#include "Window.h"

#include "Scene.h"
#include "ViewPort.h"

namespace Render
{
    Core::Common::Handle Window::AddViewPort(Core::Memory::UniquePtr<ViewPortConfig> in_upConfig)
    {
        // ビューポートの縦横サイズを調整
        // ウィンドウサイズを超えないようにする
        if (in_upConfig->uHeight <= 0)
        {
            in_upConfig->uHeight = this->_upStrategy->_uHeight;
        }
        else if (this->_upStrategy->_uHeight < in_upConfig->uHeight)
        {
            in_upConfig->uHeight = this->_upStrategy->_uHeight;
        }

        if (in_upConfig->uWidth <= 0)
        {
            in_upConfig->uWidth = this->_upStrategy->_uWidht;
        }
        else if (this->_upStrategy->_uHeight < in_upConfig->uHeight)
        {
            in_upConfig->uWidth = this->_upStrategy->_uWidht;
        }

        auto [handle, pData] = this->_poolViewPortManager.Alloc<ViewPort>();

        pData->_Setup(std::move(in_upConfig));

        return handle;
    }

    HE::Bool Window::RemoveViewPort(const Core::Common::Handle& in_rHandle)
    {
        auto pViewPort = this->_poolViewPortManager.Ref(in_rHandle);
        HE_ASSERT(pViewPort);

        pViewPort->_End();

        this->_poolViewPortManager.Free(in_rHandle, FALSE);

        return TRUE;
    }

    void Window::Show()
    {
        if (this->_bReady) this->_upStrategy->_VShow();
        this->_bShow = TRUE;
    }

    HE::Bool Window::_Setup(Core::Memory::UniquePtr<WindowStrategy> in_upConfig)
    {
        this->_upStrategy = std::move(in_upConfig);
        HE_ASSERT(0 < this->_upStrategy->ViewPortCount());

        this->_poolViewPortManager.ReleasePool();
        this->_poolViewPortManager.ReservePool(this->_upStrategy->ViewPortCount());

        return TRUE;
    }

    void Window::_Release()
    {
        this->_poolViewPortManager.ReleasePool();

        HE_SAFE_DELETE_UNIQUE_PTR(this->_upStrategy);
    }

    void Window::_Begin()
    {
        this->_upStrategy->_VBegin();
        if (this->_bShow)
        {
            this->_upStrategy->_VShow();
        }
        this->_bReady = TRUE;
    }

    void Window::_End()
    {
        this->_upStrategy->_VEnd();

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
                    itrScene->second->_VUpdate(in_fDt);
                }
            }
        }
    }

    void Window::_Render()
    {
        this->_upStrategy->_VBeginRender();

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

                for (auto itrScene = m->begin(); itrScene != m->end(); ++itrScene)
                {
                    // シーン描画
                    itrScene->second->_VRender(pViewPort);
                }
            }
        }

        this->_upStrategy->_VEndRender();
    }
}  // namespace Render
