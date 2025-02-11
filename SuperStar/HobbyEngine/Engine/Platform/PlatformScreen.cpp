#include "PlatformScreen.h"

namespace Platform
{
    void WindowStrategy::AddBeginRenderCallback(OnBeginRenderCallback in_callback)
    {
        this->_onBeginCallback = std::move(in_callback);
    }

    void WindowStrategy::AddEndRenderCallback(OnEndRenderCallback in_callback)
    {
        this->_onEndCallback = std::move(in_callback);
    }

    void WindowStrategy::AddUpdateRenderCallback(OnUpdateRenderCallback in_callback)
    {
        this->_onUpdteCallback = std::move(in_callback);
    }

    WindowStrategy::WindowStrategy(const Core::Common::Handle in_handle,
                                   const WindowConfig& in_rConfig)
    {
        this->_handle = in_handle;
        this->_config = in_rConfig;
    }

    void WindowStrategy::VRelease()
    {
        this->_onBeginCallback = NULL;
        this->_onEndCallback   = NULL;
        this->_onUpdteCallback = NULL;
    }

    ViewPortStrategy::ViewPortStrategy(const ViewPortConfig& in_rConfig)
    {
        this->_config = in_rConfig;
    }

}  // namespace Platform
