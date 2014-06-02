#include "counters.h"

#include "string_join.h"
#include <iostream>
#include <string>

namespace psz {

Counter::Counter(const std::string& name) : value_(0), name_(name) {}

void Counter::Increment() { ++value_; }

void Counter::IncrementBy(unsigned int v) { value_ += v; }

unsigned int Counter::GetValue() const { return value_; }

std::string Counter::GetName() const { return name_; }

Counter* CounterBox::GetCounter(const std::string& name) {
  if (counter_by_name_.count(name) == 0) {
    counter_by_name_.emplace(name, std::unique_ptr<Counter>(new Counter(name)));
  }
  return counter_by_name_[name].get();
}

CounterBox::data_type::const_iterator CounterBox::begin() {
  return counter_by_name_.begin();
}

CounterBox::data_type::const_iterator CounterBox::end() {
  return counter_by_name_.end();
}

CounterBox* GetCounterBox() {
  static CounterBox box;
  return &box;
}

Counter* GetCounter(const std::string& name) {
  return GetCounterBox()->GetCounter(name);
}

void PrintCounters() {
  CounterBox* box = GetCounterBox();
  for (CounterBox::data_type::const_iterator iter = box->begin();
       iter != box->end(); ++iter) {
    std::cout << iter->first << '=' << iter->second->GetValue() << std::endl;
  }
}

std::string FindBucket(double min_val, double max_val, unsigned int num_buckets,
                       double value) {
  const double p = (max_val - min_val) / num_buckets;
  int m = floor(value / p);
  if (value < min_val) {
    return StrCat("[-inf,", min_val, ")");
  } else if (value >= max_val) {
    return StrCat("[", max_val, ",inf)");
  }
  return StrCat("[", min_val + p * m, ',', min_val + p * (m + 1.0), ")");
}

}  // psz