#include "PlatformScreen.h"

namespace Platform
{
    ScreenSceneViewUIConfig::ScreenSceneViewUIConfig(const Core::Common::Handle& in_windowHandle,
                                                     const Core::Common::Handle& in_viewPortHandle)
        : _windowHandle(in_windowHandle), _viewPortHandle(in_viewPortHandle)
    {
    }

    ScreenSceneView2DConfig::ScreenSceneView2DConfig(const Core::Common::Handle& in_windowHandle,
                                                     const Core::Common::Handle& in_viewPortHandle)
        : _windowHandle(in_windowHandle), _viewPortHandle(in_viewPortHandle)
    {
    }
}  // namespace Platform
