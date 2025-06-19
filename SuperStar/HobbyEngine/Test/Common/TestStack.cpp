#include <gtest/gtest.h>

#include "Engine/Common/CustomStack.h"

namespace Core
{
    TEST(HobbyEngine_Common, TestStackUse)
    {
        Core::Common::FixedStack<HE::Sint32, 32> stack;

        EXPECT_EQ(stack.Capacity(), 32);
        EXPECT_EQ(stack.Empty(), TRUE);

        stack.PushBack(10);
        EXPECT_NE(stack.Empty(), TRUE);
        EXPECT_EQ(stack.PopBack(), 10);
        EXPECT_EQ(stack.Empty(), TRUE);

        stack.PushBack(10);
        stack.PushBack(10);
        stack.PushBack(10);
        stack.PushBack(10);
        stack.PushBack(10);
        stack.PushBack(10);
        EXPECT_NE(stack.Empty(), TRUE);
        EXPECT_EQ(stack.Size(), 6);

        stack.PopBack();
        stack.PopBack();
        stack.PopBack();
        stack.PopBack();
        EXPECT_EQ(stack.Size(), 2);

        stack.Clear();
        EXPECT_EQ(stack.Empty(), TRUE);
    }
}  // namespace Core
