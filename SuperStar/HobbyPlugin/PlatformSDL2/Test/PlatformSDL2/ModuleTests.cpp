#include <gtest/gtest.h>

#include "PlatformSDL2Module.h"

namespace PlatformSDL2
{
    TEST(HobbyPlugin_PlatformSDL2, ModuleName)
    {
        EXPECT_STREQ(PlatformSDL2Module::ModuleName(), "PlatformSDL2Module");
    }
}
