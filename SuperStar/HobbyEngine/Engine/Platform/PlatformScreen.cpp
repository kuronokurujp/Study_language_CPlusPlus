#include "PlatformScreen.h"

namespace Platform
{
    WindowStrategy::WindowStrategy(const Core::Common::Handle in_handle,
                                   const WindowConfig& in_rConfig)
    {
        this->_handle = in_handle;
        this->_config = in_rConfig;
    }

    void WindowStrategy::VRelease()
    {
    }

    ViewPortStrategy::ViewPortStrategy(const ViewPortConfig& in_rConfig)
    {
        this->_config = in_rConfig;
    }

}  // namespace Platform
