#include "SpMeanCovariance.hpp"

SpMeanCovariance::SpMeanCovariance() : d_(0), d2_(0), w_(0.) {}

SpMeanCovariance::SpMeanCovariance(int d) : d_(d), d2_(0), w_(0.) {
  ResizeInternal(d);
}

void SpMeanCovariance::Resize(int d) {
  ResizeInternal(d);
  Clear();
}

void SpMeanCovariance::Clear() {
  std::fill(t_.begin(), t_.end(), 0.);
  std::fill(dX_.begin(), dX_.end(), 0.);
  std::fill(s_.begin(), s_.end(), 0.);
  w_ = 0.;
}

void SpMeanCovariance::Combine(const SpMeanCovariance &M) {
  Add(M.w_, M.t_, M.s_);
}

void SpMeanCovariance::Add(double *x) {
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
  double f = 1. / w_ / (w_ - 1.);
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

void SpMeanCovariance::AddWithWeight(double w, double *x) {
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

  double *s_ptr = &s_[0];
  const double f = w_ * w / (w_ + w);
  for (int r = 0; r < d_; ++r) {
    for (int c = 0; c <= r; ++c) {
      *s_ptr += f * dX_[r] * dX_[c];
      ++s_ptr;
    }
  }

  for (int i = 0; i < d_; ++i) {
    t_[i] += w * x[i];
  }
  w_ += w;
}

double SpMeanCovariance::Mean(int i) const { return t_[i] / w_; }

double SpMeanCovariance::Cov(int i) const { return s_[i] / w_; }

double SpMeanCovariance::Covp(int i) const { return s_[i] / (w_ - 1.); }

double SpMeanCovariance::d() const { return d_; }

double SpMeanCovariance::d2() const { return d2_; }

double SpMeanCovariance::w() const { return w_; }

//
// Private
//
void SpMeanCovariance::ResizeInternal(int d) {
  d_ = d;
  d2_ = (d * (d + 1) / 2);

  t_.resize(d, 0.);
  dX_.resize(d, 0.);
  s_.resize(d * (d + 1) / 2, 0.);
}

void SpMeanCovariance::Add(double other_w, const std::vector<double> &other_t,
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