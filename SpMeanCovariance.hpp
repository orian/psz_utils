// Chan, Tony F.; Golub, Gene H.; LeVeque, Randall J. (1979), “Updating Formulae
// and a Pairwise Algorithm for Computing Sample Variances.”, Technical Report
// STAN-CS-79-773, Department of Computer Science, Stanford University.

#ifndef PARALLEL_VARIANCE_SP_MEAN_COVARIANCE_HPP
#define PARALLEL_VARIANCE_SP_MEAN_COVARIANCE_HPP

#include <vector>

class SpMeanCovariance {
 public:
  SpMeanCovariance();

  SpMeanCovariance(int d);

  void Resize(int d);

  void Clear();

  void Combine(const SpMeanCovariance &M);

  void Add(double *x);

  void AddWithWeight(double w, double *x);

  double Mean(int i) const;

  double Cov(int i) const;

  double Covp(int i) const;

  double d() const;

  double d2() const;

  double w() const;

 private:
  void ResizeInternal(int d);
  void Add(double other_w, const std::vector<double> &other_t,
           const std::vector<double> &other_s);

  int d_;
  int d2_;
  double w_;
  std::vector<double> t_;
  std::vector<double> s_;
  std::vector<double> dX_;
};

#endif  // PARALLEL_VARIANCE_SP_MEAN_COVARIANCE_HPP
