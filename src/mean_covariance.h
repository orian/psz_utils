// Chan, Tony F.; Golub, Gene H.; LeVeque, Randall J. (1979), “Updating Formulae
// and a Pairwise Algorithm for Computing Sample Variances.”, Technical Report
// STAN-CS-79-773, Department of Computer Science, Stanford University.

#ifndef PARALLEL_VARIANCE_SP_MEAN_COVARIANCE_HPP
#define PARALLEL_VARIANCE_SP_MEAN_COVARIANCE_HPP

#include <algorithm>
#include <vector>

namespace psz {

class MeanCovariance {
 public:
  MeanCovariance();

  MeanCovariance(int d);

  void Resize(int d);

  void Clear();

  void Combine(const MeanCovariance &M);

  void Add(const double *x);

  void AddBatch(int batch_size, const double *x);

  void AddWithWeight(double w, const double *x);

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

/*
function KahanSum(input)
    var sum = 0.0
    var c = 0.0                  // A running compensation for lost low-order
bits.
    for i = 1 to input.length do
        var y = input[i] - c     // So far, so good: c is zero.
        var t = sum + y          // Alas, sum is big, y small, so low-order
digits of y are lost.
        c = (t - sum) - y // (t - sum) recovers the high-order part of y;
subtracting y recovers -(low part of y)
        sum = t           // Algebraically, c should always be zero. Beware
overly-aggressive optimizing compilers!
        // Next time around, the lost low part will be added to y in a fresh
attempt.
    return sum
 */
// size = row*row_number
template <typename InT, typename OutT>
void KahanSum(const InT *input, OutT *output, int row, int row_number) {
  std::fill(output, output + row, 0.);
  std::vector<OutT> c(row, 0.);
  OutT y = 0.;
  OutT t = 0.;
  for (int i = 0; i < row_number; ++i) {
    for (int j = 0; j < row; ++j) {
      OutT y = OutT(*input) - c[j];
      OutT t = output[j] + y;
      c[j] = (t - output[j]) - y;
      output[j] = t;
      ++input;
    }
  }
}

template <typename T>
void MeanF(int vars, int sample_num, const T *input,
           std::vector<double> *means) {
  means->resize(vars);
  //  std::fill(means->begin(), means->end(), 0.);

  KahanSum<T, double>(input, &(*means)[0], vars, sample_num);
  std::transform(means->begin(), means->end(), means->begin(),
                 [sample_num](double a) -> double { return a / sample_num; });
}

/**
 * Computes mean and covariance between variable.
 * @param[in] vars number of variables
 * @param[in] sample_num number of data samples
 * @param[in] x data samples
 * @param[in] unbiased should use N as denominator for variance
 * @param[out] means means for each variable
 * @param[out] covs covariances between variables,
 */
template <typename T>
void MeanCovarianceF(int vars, int sample_num, const T *input, bool unbiased,
                     std::vector<double> *means, std::vector<double> *covs) {
  //  std::vector<double> means(vars, 0.);
  //  std::vector<double> covs(vars, 0.);
  MeanF(vars, sample_num, input, means);

  int covs_num = vars * (vars + 1) / 2;
  covs->resize(covs_num);
  std::fill(covs->begin(), covs->end(), 0.);

  for (int i = 0; i < sample_num; ++i) {
    auto covs_iter = covs->begin();
    for (int j = 0; j < vars; ++j) {
      for (int k = 0; k <= j; ++k) {
        auto c = (input[j] - (*means)[j]) * (input[k] - (*means)[k]);
        *covs_iter += c;
        ++covs_iter;
      }
    }
    input += vars;
  }
  double den = unbiased ? sample_num : (sample_num - 1);
  std::transform(covs->begin(), covs->end(), covs->begin(),
                 [den](double a) -> double { return a / den; });

  // setting values
}

}  // namespace psz

#endif  // PARALLEL_VARIANCE_SP_MEAN_COVARIANCE_HPP
