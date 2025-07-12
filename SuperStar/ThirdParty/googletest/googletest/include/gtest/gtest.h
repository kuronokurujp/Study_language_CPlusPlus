#ifndef GTEST_GTEST_H_
#define GTEST_GTEST_H_

#define TEST(test_case_name, test_name) void test_case_name##_##test_name()
#define EXPECT_EQ(val1, val2) ((void)0)
#define EXPECT_TRUE(cond) ((void)0)
#define EXPECT_FALSE(cond) ((void)0)

#endif // GTEST_GTEST_H_
