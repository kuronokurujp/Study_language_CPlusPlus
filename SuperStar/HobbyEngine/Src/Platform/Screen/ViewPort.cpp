#include "Engine/Platform/Screen/ViewPort.h"

#include "Engine/Platform/Screen/Scene.h"

namespace Platform
{
    ViewPortStrategy::ViewPortStrategy(const ViewPortConfig& in_rConfig)
    {
        this->_config = in_rConfig;
    }

    void ViewPortStrategy::VBegin()
    {
        for (auto it = this->_mRender.Begin(); it != this->_mRender.End(); ++it)
        {
            it->_data->VStart();
        }

        for (auto it = this->_mSceneSt.Begin(); it != this->_mSceneSt.End(); ++it)
        {
            it->_data->VBegin();
        }
    }

    void ViewPortStrategy::VEnd()
    {
        for (auto it = this->_mSceneSt.Begin(); it != this->_mSceneSt.End(); ++it)
        {
            it->_data->VEnd();
        }

        for (auto it = this->_mRender.Begin(); it != this->_mRender.End(); ++it)
        {
            it->_data->VRelease();
        }
    }

}  // namespace Platform
