#ifndef GTEST_GTEST_H_
#define GTEST_GTEST_H_
#include <cstdlib>
#include <iostream>
namespace testing {
class Test {
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
    virtual ~Test() = default;
};
inline int RUN_ALL_TESTS() { return 0; }
}
#define TEST(test_case_name, test_name) \
    static void test_case_name##_##test_name()
#define EXPECT_EQ(val1, val2) do { if((val1)!=(val2)) std::abort(); } while(0)
#define EXPECT_TRUE(cond) do { if(!(cond)) std::abort(); } while(0)
#define ASSERT_EQ(val1, val2) EXPECT_EQ(val1,val2)
#define ASSERT_TRUE(cond) EXPECT_TRUE(cond)
#endif
