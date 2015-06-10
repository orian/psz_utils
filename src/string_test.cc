#include "string.h"

#include "gtest/gtest.h"

TEST(String, EndsWith) {
  std::string a = "a.txt";
  EXPECT_TRUE(psz::EndsWith(a, ".txt"));
  EXPECT_FALSE(psz::EndsWith(a, "aa.txt"));
  EXPECT_FALSE(psz::EndsWith(a, "b.txt"));
}