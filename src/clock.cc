#include "clock.h"

namespace psz {

Clock::Clock() : start_time_(std::clock()) {}

std::clock_t Clock::Now() const { return std::clock() - start_time_; }

double Clock::NowSeconds() const {
  return static_cast<double>(std::clock() - start_time_) / CLOCKS_PER_SEC;
}

void Clock::Reset() { start_time_ = std::clock(); }

}  // namespace psz