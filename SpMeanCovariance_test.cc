#include "SpMeanCovariance.hpp"

#include "gtest/gtest.h"

#include <cmath>

TEST(SpMeanCovariance, Simple) {
  {
    int N = 10;
    double x[] = {4., 1., 9., 4., 9., 6., 8., 9., 5., 5.};
    double ans_mu = 6;
    double ans_var = 6.6;

    SpMeanCovariance M(1);

    int i = 0;
    for (i = 0; i < N; ++i) {
      M.Add(x + i);
    }

    auto cmu = M.Mean(0);
    auto ccov = M.Cov(0);
    EXPECT_LT(std::abs(ans_mu - cmu), 0.0001);
    EXPECT_LT(std::abs(ans_var - ccov), 0.0001);
  }
}

TEST(SpMeanCovariance, MultiVariable) {
  int N = 10;
  int D = 3;

  double x[] = {5., 7., 1., 7., 8., 3., 8., 4., 2., 7., 2., 8., 1., 7., 3.,
                3., 8., 6., 2., 7., 5., 6., 5., 7., 9., 1., 8., 6., 7., 1.};

  double ans_mu[] = {5.4, 5.6, 4.4};

  double ans_cov[] = {6.24, -3.94, 5.64, 1.14, -3.74, 6.84};

  SpMeanCovariance M(3);
  for (int i = 0; i < N; ++i) {
    M.Addw(1., &x[i * D]);
  }
  EXPECT_FLOAT_EQ(N, M.w());
  for (int i = 0; i < M.d(); ++i) {
    EXPECT_FLOAT_EQ(ans_mu[i], M.Mean(i));
  }
  for (int i = 0; i < M.d2(); ++i) {
    EXPECT_FLOAT_EQ(ans_cov[i],M.Cov(i));
  }
}

TEST(SpMeanCovariance, Parallel) {
  int N = 10;
  int D = 3;

  double x[] = {
          5., 7., 1.,
          7., 8., 3.,
          8., 4., 2.,
          7., 2., 8.,
          1., 7., 3.,
          3., 8., 6.,
          2., 7., 5.,
          6., 5., 7.,
          9., 1., 8.,
          6., 7., 1.
  };

  double ans_mu[] = { 5.4, 5.6, 4.4 };

  double ans_cov[] = {
          6.24,
          -3.94, 5.64,
          1.14,-3.74, 6.84
  };

  SpMeanCovariance M(3);
  SpMeanCovariance M2(3);
  for (int i=0; i<6; ++i) {
    M.Addw(1., &x[i*D] );
  }
  for (int i=6; i<N; ++i) {
    M2.Addw(1., &x[i*D] );
  }
  M.Combine(M2);

  EXPECT_FLOAT_EQ(N, M.w());
  for (int i = 0; i < M.d(); ++i) {
    EXPECT_FLOAT_EQ(ans_mu[i], M.Mean(i));
  }
  for (int i = 0; i < M.d2(); ++i) {
    EXPECT_FLOAT_EQ(ans_cov[i],M.Cov(i));
  }
}
