#include <gtest/gtest.h>

#include "Engine/Common/Hash.h"
#include "Engine/Macro.h"

namespace Core
{
    // ハッシュ値がかぶっていないか
    TEST(HobbyEngine_Common, TestHashDuplicationCheck)
    {
        const HE::Uint32 uCount          = 3;
        const HE::Char* aszTexts[uCount] = {HE_STR_TEXT("a"), HE_STR_TEXT("b"), HE_STR_TEXT("c")};
        HE::Uint64 auuHash[uCount]       = {};

        HE_LOG_LINE(HE_STR_EMPTY);
        for (auto i = 0; i < HE_ARRAY_NUM(aszTexts); ++i)
        {
            auuHash[i] = Core::Common::HashName(aszTexts[i]);
            HE_LOG_LINE(HE_STR_TEXT("text: %s to hash: %lld %s"), aszTexts[i], auuHash[i],
                        HE_STR_TEXT("日本"));
        }

        for (auto i = 0; i < HE_ARRAY_NUM(aszTexts); ++i)
        {
            auto hash = auuHash[i];
            for (auto j = i + 1; j < HE_ARRAY_NUM(aszTexts); ++j)
            {
                EXPECT_NE(hash, auuHash[j]);
            }
        }
    }
}  // namespace Core
