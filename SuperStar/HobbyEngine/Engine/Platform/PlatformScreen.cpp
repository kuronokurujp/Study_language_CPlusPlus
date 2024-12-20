#include "PlatformScreen.h"

namespace Platform
{
    WindowStrategy::WindowStrategy(const WindowConfig& in_rConfig)
    {
        this->_config = in_rConfig;
    }

    ViewPortStrategy::ViewPortStrategy(const ViewPortConfig& in_rConfig)
    {
        this->_config = in_rConfig;
    }

}  // namespace Platform
