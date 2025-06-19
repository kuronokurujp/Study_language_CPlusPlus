#include <gtest/gtest.h>

#include "Engine/Common/UUID.h"
#include "Engine/Core.h"

namespace Core
{
    TEST(HobbyEngine_Common, MacroUUID32Bit)
    {
        static const HE::Uint32 s_uUUIDCheckCount = 100000;
        HE::Uint32 uHitCount                      = 0;
        HE::Uint32 uCount                         = 0;
        HE::Uint32 uaUUIDValue[s_uUUIDCheckCount] = {0};
        for (HE::Uint32 i = 0; i < s_uUUIDCheckCount; ++i)
        {
            HE::Uint32 uValue = Core::Common::GenerateUUIDBy32Bit();
            for (HE::Uint32 j = 0; j < uCount; ++j)
            {
                if (uValue == uaUUIDValue[j])
                {
                    ++uHitCount;
                    break;
                }
            }

            uaUUIDValue[i] = uValue;
            ++uCount;
        }
        // 100000回繰り返してUUIDを生成すると一度だけ値が衝突した
        HE_LOG_LINE(HE_STR_TEXT("32bitのUUID生成を(%d)回繰り返した結果, 値の衝突回数(%d)だった"),
                    s_uUUIDCheckCount, uHitCount);
    }
}  // namespace Core
