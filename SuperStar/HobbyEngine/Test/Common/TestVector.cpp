#include <gtest/gtest.h>

#include "Engine/Common/CustomVector.h"

namespace Core
{
    TEST(HobbyEngine_Common, CustomVectorUse)
    {
        Core::Common::FixedVector<HE::Sint32, 32> vector;

        EXPECT_EQ(vector.Capacity(), 32);
        vector.PopBack();
        EXPECT_EQ(vector.Size(), 0);

        vector.PushBack(10);
        EXPECT_EQ(vector[0], 10);
        vector.PushBack(20);
        EXPECT_EQ(vector[1], 20);
        vector.PushBack(30);
        EXPECT_EQ(vector[2], 30);
        vector.PushBack(40);
        EXPECT_EQ(vector[3], 40);
        EXPECT_EQ(vector.Size(), 4);

        vector.Remove(20);
        EXPECT_EQ(vector[2], 40);
        EXPECT_EQ(vector.Size(), 3);
        vector.PushBack(30);
        EXPECT_EQ(vector[3], 30);

        while (vector.Empty() == FALSE) vector.PopBack();

        EXPECT_EQ(vector.Empty(), TRUE);
    }

    TEST(HobbyEngine_Common, CustomVectorInsert)
    {
        Core::Common::FixedVector<HE::Sint32, 32> vector;

        EXPECT_EQ(vector.Capacity(), 32);
        vector.PopBack();
        EXPECT_EQ(vector.Size(), 0);

        vector.PushBack(10);
        EXPECT_EQ(vector[0], 10);
        vector.PushBack(20);
        EXPECT_EQ(vector[1], 20);

        vector.Insert(1, 30);
        EXPECT_EQ(vector[1], 30);
        EXPECT_EQ(vector[2], 20);
        EXPECT_EQ(vector[0], 10);

        {
            // 末尾に挿入
            const HE::Uint32 i = vector.Size();
            vector.Insert(i, 999);
            // 末尾に正しい値に入っているか
            EXPECT_EQ(vector[i], 999);
        }
    }

    TEST(HobbyEngine_Common, CustomVectorCopy)
    {
        // vector同士のコピー
        Core::Common::FixedVector<HE::Sint32, 32> src;
        src.PushBack(10);
        src.PushBack(-10);

        Core::Common::FixedVector<HE::Sint32, 32> copy;
        copy = src;

        EXPECT_EQ(copy[0], 10);
        EXPECT_EQ(copy[1], -10);
    }
}  // namespace Core
