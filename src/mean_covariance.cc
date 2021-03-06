#include "mean_covariance.h"

#include <algorithm>
#include <iostream>

namespace psz {

MeanCovariance::MeanCovariance() : d_(0), d2_(0), w_(0.) {}

MeanCovariance::MeanCovariance(int d) : d_(d), d2_(0), w_(0.) {
  ResizeInternal(d);
}

void MeanCovariance::Resize(int d) {
  ResizeInternal(d);
  Clear();
}

void MeanCovariance::Clear() {
  std::fill(t_.begin(), t_.end(), 0.);
  std::fill(dX_.begin(), dX_.end(), 0.);
  std::fill(s_.begin(), s_.end(), 0.);
  w_ = 0.;
}

void MeanCovariance::Combine(const MeanCovariance &M) { Add(M.w_, M.t_, M.s_); }

void MeanCovariance::Add(const double *x) {
  if (w_ == 0.) {
    w_ = 1.;
    for (int i = 0; i < d_; ++i) t_[i] = x[i];
    for (int i = 0; i < d2_; ++i) s_[i] = 0.;
    return;
  }
  w_ += 1.;
  for (int i = 0; i < d_; ++i) {
    t_[i] += x[i];
  }

  auto s_iter = s_.begin();
  const double f = 1. / w_ / (w_ - 1.);
  for (int i = 0; i < d_; ++i) {
    dX_[i] = w_ * x[i] - t_[i];
  }

  for (int r = 0; r < d_; ++r) {
    for (int c = 0; c <= r; ++c) {
      *s_iter += f * dX_[r] * dX_[c];
      ++s_iter;
    }
  }
}

void MeanCovariance::AddBatch(int batch_size, const double *x) {
  std::vector<double> means(d_, 0.);
  std::vector<double> covs(d_, 0.);
  KahanSum(x, &means[0], d_, batch_size);
  std::transform(means.begin(), means.end(), means.begin(),
                 [batch_size](double a) -> double { return a / batch_size; });
  for (int i = 0; i < batch_size; ++i) {
    int l = 0;
    for (int j = 0; j < d_; ++j) {
      for (int k = 0; k < j; ++k) {
        covs[l] += (x[i * batch_size + j] - means[l]) *
                   (x[i * batch_size + k] - means[k]);
      }
    }
  }
  std::transform(
      covs.begin(), covs.end(), covs.begin(),
      [batch_size](double a) -> double { return a / (batch_size - 1); });

  // setting values
}

void MeanCovariance::AddWithWeight(double w, const double *x) {
  //--------------------------------
  // add( w, w*x, 0. );
  //--------------------------------
  if (w_ == 0.) {
    w_ = w;
    for (int i = 0; i < d_; ++i) t_[i] = w * x[i];
    std::fill(s_.begin(), s_.end(), 0.);
    return;
  }

  for (int i = 0; i < d_; ++i) {
    dX_[i] = x[i] - t_[i] / w_;
  }

  auto s_iter = s_.begin();
  const double f = w_ * w / (w_ + w);
  for (int r = 0; r < d_; ++r) {
    for (int c = 0; c <= r; ++c) {
      *s_iter += f * dX_[r] * dX_[c];
      ++s_iter;
    }
  }

  for (int i = 0; i < d_; ++i) {
    t_[i] += w * x[i];
  }
  w_ += w;
}

double MeanCovariance::Mean(int i) const { return t_[i] / w_; }

double MeanCovariance::Cov(int i) const { return s_[i] / w_; }

double MeanCovariance::Covp(int i) const { return s_[i] / (w_ - 1.); }

double MeanCovariance::d() const { return d_; }

double MeanCovariance::d2() const { return d2_; }

double MeanCovariance::w() const { return w_; }

//
// Private
//
void MeanCovariance::ResizeInternal(int d) {
  d_ = d;
  d2_ = (d * (d + 1) / 2);

  t_.resize(d, 0.);
  dX_.resize(d, 0.);
  s_.resize(d * (d + 1) / 2, 0.);
}

void MeanCovariance::Add(double other_w, const std::vector<double> &other_t,
                         const std::vector<double> &other_s) {
  if (w_ == 0.) {
    w_ = other_w;
    std::copy(other_t.begin(), other_t.end(), t_.begin());
    std::copy(other_s.begin(), other_s.end(), s_.begin());
    return;
  }

  for (int i = 0; i < d_; ++i) {
    dX_[i] = other_t[i] / other_w - t_[i] / w_;
  }
  auto s_iter = s_.begin();
  auto other_s_iter = other_s.begin();
  double f = w_ * other_w / (w_ + other_w);
  for (int r = 0; r < d_; ++r) {
    for (int c = 0; c <= r; ++c) {
      *s_iter += *other_s_iter + f * dX_[r] * dX_[c];
      ++s_iter;
      ++other_s_iter;
    }
  }

  for (int i = 0; i < d_; ++i) t_[i] += other_t[i];
  w_ += other_w;
}

}  // namespace psz
