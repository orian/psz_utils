#include "mean_covariance.h"

#include "gtest/gtest.h"

#include <cmath>

TEST(MeanCovariance, Simple) {
  {
    int N = 10;
    double x[] = {4., 1., 9., 4., 9., 6., 8., 9., 5., 5.};
    double ans_mu = 6;
    double ans_var = 6.6;

    MeanCovariance m(1);

    int i = 0;
    for (i = 0; i < N; ++i) {
      m.Add(x + i);
    }

    auto cmu = m.Mean(0);
    auto ccov = m.Cov(0);
    EXPECT_LT(std::abs(ans_mu - cmu), 0.0001);
    EXPECT_LT(std::abs(ans_var - ccov), 0.0001);
  }
}

TEST(MeanCovariance, MultiVariable) {
  int N = 10;
  int D = 3;

  double x[] = {5., 7., 1., 7., 8., 3., 8., 4., 2., 7., 2., 8., 1., 7., 3.,
                3., 8., 6., 2., 7., 5., 6., 5., 7., 9., 1., 8., 6., 7., 1.};

  double ans_mu[] = {5.4, 5.6, 4.4};

  double ans_cov[] = {6.24, -3.94, 5.64, 1.14, -3.74, 6.84};

  MeanCovariance m(3);
  for (int i = 0; i < N; ++i) {
    m.AddWithWeight(1., &x[i * D]);
  }
  EXPECT_FLOAT_EQ(N, m.w());
  for (int i = 0; i < m.d(); ++i) {
    EXPECT_FLOAT_EQ(ans_mu[i], m.Mean(i));
  }
  for (int i = 0; i < m.d2(); ++i) {
    EXPECT_FLOAT_EQ(ans_cov[i], m.Cov(i));
  }
}

TEST(MeanCovariance, Parallel) {
  int N = 10;
  int D = 3;

  double x[] = {5., 7., 1., 7., 8., 3., 8., 4., 2., 7., 2., 8., 1., 7., 3.,
                3., 8., 6., 2., 7., 5., 6., 5., 7., 9., 1., 8., 6., 7., 1.};

  double ans_mu[] = {5.4, 5.6, 4.4};

  double ans_cov[] = {6.24, -3.94, 5.64, 1.14, -3.74, 6.84};

  MeanCovariance m(3);
  MeanCovariance m2(3);
  for (int i = 0; i < 6; ++i) {
    m.AddWithWeight(1., &x[i * D]);
  }
  for (int i = 6; i < N; ++i) {
    m2.AddWithWeight(1., &x[i * D]);
  }
  m.Combine(m2);

  EXPECT_FLOAT_EQ(N, m.w());
  for (int i = 0; i < m.d(); ++i) {
    EXPECT_FLOAT_EQ(ans_mu[i], m.Mean(i));
  }
  for (int i = 0; i < m.d2(); ++i) {
    EXPECT_FLOAT_EQ(ans_cov[i], m.Cov(i));
  }
}

TEST(MeanCovariance, NumpySimple) {
  /* Python code used to generate values.
   import numpy as np
   data = np.arange(1,100)
   ans_mu = np.mean(data)
   ans_var = np.cov(data)
   */
  double ans_mu = 50.;
  double ans_var = 816.666666;
  MeanCovariance m(1);
  for (int i = 1; i < 100; ++i) {
    double x = i;
    m.Add(&x);
  }
  EXPECT_FLOAT_EQ(ans_mu, m.Mean(0));
  EXPECT_FLOAT_EQ(ans_var, m.Cov(0));
}

TEST(MeanCovariance, NumpyMultiVariables) {
  /* Python code used to generate values.
   import numpy as np
   data = np.arange(1,100)
   data3v=np.array([data, data*2+1, data/.3-5])
   ans_mu = np.mean(data3v, axis=1)
   ans_var = np.cov(data3v)
   */
  double ans_mu[] = {50., 101., 161.66666667};
  double ans_cov[] = {825., 1650., 3300., 2750., 5500., 9166.66666667};
  MeanCovariance m(1);
  for (int i = 1; i < 100; ++i) {
      double v=i;
    double x[] = {v, v * 2 + 1, v / .3 - 5};
    m.Add(x);
  }
  EXPECT_FLOAT_EQ(99, m.w());
  for (int i = 0; i < m.d(); ++i) {
    EXPECT_FLOAT_EQ(ans_mu[i], m.Mean(i));
  }
  for (int i = 0; i < m.d2(); ++i) {
    EXPECT_FLOAT_EQ(ans_cov[i], m.Covp(i));
  }
};