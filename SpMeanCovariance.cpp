#include "SpMeanCovariance.hpp"

SpMeanCovariance::SpMeanCovariance() : d_(0), d2_(0), w_(0.) {}

SpMeanCovariance::SpMeanCovariance(int d) : d_(d), d2_(0), w_(0.) { ResizeInternal(d); }

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

void SpMeanCovariance::Combine(SpMeanCovariance &M) { Add(M.w_, &M.t_[0], &M.s_[0]); }

void SpMeanCovariance::Add(double W_, double *T_, double *S_) {
    if (w_ == 0.) {
        w_ = W_;
//      std::copy(T_, T_+d_, t_);
//      std::copy(S_, S_+d2_, s_);
        for (int i = 0; i < d_; ++i) t_[i] = T_[i];
        for (int i = 0; i < d2_; ++i) s_[i] = S_[i];
        return;
    }
    for (int i = 0; i < d_; ++i) dX_[i] = T_[i] / W_ - t_[i] / w_;

    double *Si = &s_[0];
    double *Si_ = &S_[0];
    double f = w_ * W_ / (w_ + W_);
    for (int r = 0; r < d_; ++r) {
        for (int c = 0; c <= r; ++c) {
            *Si += *Si_ + f * dX_[r] * dX_[c];
            Si++;
            Si_++;
        }
    }

    for (int i = 0; i < d_; ++i) t_[i] += T_[i];
    w_ += W_;
}

void SpMeanCovariance::Addw(double w, double *x) {
    //--------------------------------
    // add( w, w*x, 0. );
    //--------------------------------
    if (w_ == 0.) {
        w_ = w;
        for (int i = 0; i < d_; ++i) t_[i] = w * x[i];
        for (int i = 0; i < d2_; ++i) s_[i] = 0.;
        return;
    }

    for (int i = 0; i < d_; ++i) {
        dX_[i] = x[i] - t_[i] / w_;
    }

    double *s_ptr = &s_[0];
    double f = w_ * w / (w_ + w);
    for (int r = 0; r < d_; ++r) {
        for (int c = 0; c <= r; ++c) {
            *s_ptr += f * dX_[r] * dX_[c];
            ++s_ptr;
        }
    }

    for (int i = 0; i < d_; ++i) t_[i] += w * x[i];
    w_ += w;
}

double SpMeanCovariance::Mean(int i) const { return t_[i] / w_; }

double SpMeanCovariance::Cov(int i) const { return s_[i] / w_; }

double SpMeanCovariance::Covp(int i) const { return s_[i] / (w_ - 1.); }

double SpMeanCovariance::d() const { return d_; }
double SpMeanCovariance::d2() const { return d2_; }
double SpMeanCovariance::w() const { return w_; }

void SpMeanCovariance::ResizeInternal(int d) {
    d_ = d;
    d2_ = (d * (d + 1) / 2);

    t_.resize(d, 0.);
    dX_.resize(d, 0.);
    s_.resize(d * (d + 1) / 2, 0.);
}