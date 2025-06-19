#include <gtest/gtest.h>

#include "Engine/Macro.h"
#include "Engine/Str.h"

namespace Core
{
    TEST(HobbyEngine_Common, MacroText)
    {
        // 上限値の制御
        EXPECT_EQ(HE_MAX(3, 4), 4);
        // 下限値の制御
        EXPECT_EQ(HE_MIN(3, 4), 3);

        // 上限値の制御
        EXPECT_EQ(HE_MAX(15, 10), 15);
        // 下限値の制御
        EXPECT_EQ(HE_MIN(15, 10), 10);

        // 上限値の制御
        EXPECT_EQ(HE_MAX(1, 1), 1);
        // 下限値の制御
        EXPECT_EQ(HE_MIN(1, 1), 1);

        // 値のループ
        EXPECT_EQ(HE_LOOP_IN_RANGE(4, 0, 3), 0);
        EXPECT_EQ(HE_LOOP_IN_RANGE(3, 0, 3), 3);
        EXPECT_EQ(HE_LOOP_IN_RANGE(0, 0, 3), 0);
        EXPECT_EQ(HE_LOOP_IN_RANGE(1, 0, 3), 1);
        EXPECT_LE(3.2f, HE_LOOP_IN_RANGE(0.3f, 0.34f, 3.5f));
        EXPECT_EQ(HE_LOOP_IN_RANGE(0.35f, 0.34f, 3.5f), 0.35f);
        EXPECT_LE(HE_LOOP_IN_RANGE(3.6f, 0.34f, 3.5f), 0.44f);

        // 配列のサイズや要素数のチェック
        HE::Uint32 aArray[32] = {0};
        EXPECT_EQ(HE_ARRAY_NUM(aArray), 32);
        EXPECT_EQ(HE_ARRAY_SIZE(aArray), (4 * 32));
    }
}  // namespace Core
