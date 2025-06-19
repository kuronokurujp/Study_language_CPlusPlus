#include <gtest/gtest.h>

#include "Engine/Common/CustomArray.h"

namespace Core
{
    TEST(HobbyEngine_Common, TestArray)
    {
        Common::FixedArray<HE::Sint32, 32> arr;

        EXPECT_EQ(arr.Capacity(), 32);

        arr.Set(0, 10);
        EXPECT_EQ(arr[0], 10);

        arr.Set(1, 20);
        EXPECT_EQ(arr[1], 20);
        arr.Set(2, 30);
        EXPECT_EQ(arr[2], 30);
        arr.Set(3, 40);
        EXPECT_EQ(arr[3], 40);

        arr.Set(0, 5);
        EXPECT_EQ(arr[0], 5);
    }
}  // namespace Core
