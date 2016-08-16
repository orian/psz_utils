#include "string.h"

#include "gtest/gtest.h"

TEST(String, EndsWith) {
  std::string a = "a.txt";
  EXPECT_TRUE(psz::EndsWith(a, ".txt"));
  EXPECT_FALSE(psz::EndsWith(a, "aa.txt"));
  EXPECT_FALSE(psz::EndsWith(a, "b.txt"));
}

TEST(String, StartsWith) {
  std::string a = "a.txt";
  EXPECT_TRUE(psz::StartsWith(a, "a.t"));
  EXPECT_FALSE(psz::StartsWith(a, "aa.txt"));
  EXPECT_FALSE(psz::StartsWith(a, "a.txx"));
}