#include "counters.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(Bucket, Trivial) {
  EXPECT_EQ("[0,1)", psz::FindBucket(0, 10, 10, 0));
  EXPECT_EQ("[0,1)", psz::FindBucket(0, 10, 10, 0.1));
  EXPECT_EQ("[0,1)", psz::FindBucket(0, 10, 10, 0.99));
  EXPECT_EQ("[1,2)", psz::FindBucket(0, 10, 10, 1.59));
}

TEST(Bucket, BelowRange) {
  EXPECT_EQ("[-inf,0)", psz::FindBucket(0, 10, 10, -1));
}

TEST(Bucket, AboveRange) {
  EXPECT_EQ("[10,inf)", psz::FindBucket(0, 10, 10, 10));
  EXPECT_EQ("[10,inf)", psz::FindBucket(0, 10, 10, 11));
}
