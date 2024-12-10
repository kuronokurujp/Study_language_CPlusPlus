#include "ViewPort.h"

namespace Render
{
    void ViewPort::RemoveScene(Core::Common::Handle& in_rHandle)
    {
        this->_Free(in_rHandle, FALSE);
    }

    SceneViewBase* ViewPort::GetScene(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT_RETURN_VALUE(NULL, in_rHandle.Null() == FALSE);

        auto pScene = this->_Ref(in_rHandle);
        HE_ASSERT_RETURN_VALUE(NULL, pScene);

        return pScene;
    }

    const ViewPortConfig* ViewPort::GetConfig()
    {
        return this->_upConfig.get();
    }

    HE::Bool ViewPort::_Setup(Core::Memory::UniquePtr<ViewPortConfig> in_upConfig)
    {
        this->_upConfig = std::move(in_upConfig);
        this->_ReleasePool();
        this->_ReservePool(this->_upConfig->uSceneCount);

        return TRUE;
    }

    void ViewPort::_Begin()
    {
    }

    void ViewPort::_End()
    {
        {
            auto m = this->GetUserDataList();
            if (m)
            {
                for (auto itr = m->begin(); itr != m->end(); ++itr)
                {
                    itr->second->_VEnd();
                }
            }
        }
        this->_ReleasePool();

        HE_SAFE_DELETE_UNIQUE_PTR(this->_upConfig);
    }

}  // namespace Render
