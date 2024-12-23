#include "ViewPort.h"

#include "Engine/Platform/PlatformModule.h"

namespace Render
{
    std::tuple<Core::Common::Handle, SceneViewBase*> ViewPort::AddSceneView(
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> in_upStorategy)
    {
        auto [handle, pScene] = this->_poolSceneManager.Alloc<SceneViewBase>();

        pScene->_Begin(std::move(in_upStorategy));

        return std::tuple<Core::Common::Handle, SceneViewBase*>(handle, pScene);
    }

    void ViewPort::RemoveScene(Core::Common::Handle& in_rHandle)
    {
        this->_poolSceneManager.Free(in_rHandle, FALSE);
    }

    SceneViewBase* ViewPort::GetScene(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(NULL, in_rHandle.Null() == FALSE);

        auto pScene = this->_poolSceneManager.Ref(in_rHandle);
        HE_ASSERT_RETURN_VALUE(NULL, pScene);

        return pScene;
    }

    const HE::Uint32 ViewPort::Width() const
    {
        return this->_upStrategy->GetConfig()._uWidth;
    }

    const HE::Uint32 ViewPort::Height() const
    {
        return this->_upStrategy->GetConfig()._uHeight;
    }

    HE::Bool ViewPort::Init(Core::Memory::UniquePtr<Platform::ViewPortStrategy> in_upStg,
                            const HE::Uint32 in_uSceneCount)
    {
        this->_upStrategy = std::move(in_upStg);

        this->_poolSceneManager.ReleasePool();
        this->_poolSceneManager.ReservePool(in_uSceneCount);

        return TRUE;
    }

    void ViewPort::Release()
    {
        this->_poolSceneManager.ReleasePool([](SceneViewBase* in_pScene) { in_pScene->Release(); });
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upStrategy);
    }

    void ViewPort::_Begin()
    {
    }

    void ViewPort::_End()
    {
        {
            auto m = this->_poolSceneManager.GetUserDataList();
            if (m)
            {
                for (auto itr = m->begin(); itr != m->end(); ++itr)
                {
                    itr->second->_End();
                }
            }
        }
    }

    void ViewPort::_BeginRender()
    {
        this->_upStrategy->VBeginRender();
    }

    void ViewPort::_EndRender()
    {
        this->_upStrategy->VEndRender();
    }

}  // namespace Render
