#include <gtest/gtest.h>
#include "LMUtil.h"

TEST(Add, testadd) {
    EXPECT_EQ(LMUtil::add(2, 3), 5);
}