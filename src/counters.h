#ifndef __COUNTERS_H
#define __COUNTERS_H

#include <cmath>
#include <map>
#include <memory>
#include <string>

namespace psz {

class Counter {
 public:
  Counter(const std::string& name);
  void Increment();
  void IncrementBy(unsigned int v);
  unsigned int GetValue() const;
  std::string GetName() const;

 private:
  unsigned int value_;
  std::string name_;
};

class CounterBox {
 public:
  Counter* GetCounter(const std::string& name);
  typedef std::map<std::string, std::unique_ptr<Counter>> data_type;
  data_type::const_iterator begin();
  data_type::const_iterator end();

 private:
  std::map<std::string, std::unique_ptr<Counter>> counter_by_name_;
};

CounterBox* GetCounterBox();

Counter* GetCounter(const std::string& name);

void PrintCounters();

// For a range [start, end], we have N+1 buckets,
// p = (max_val-min_val)/num_buckets:
// [-inf, start), [start, p), ..., [max_val, +inf),
std::string FindBucket(double min_val, double max_val, unsigned int num_buckets,
                       double value);

}  // namespace

#endif  // __COUNTERS_H