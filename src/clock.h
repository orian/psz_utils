#ifndef __CLOCK_H
#define __CLOCK_H

#include <ctime>

namespace psz {

class Clock {
 public:
  Clock();

  std::clock_t Now() const;

  double NowSeconds() const;

  void Reset();

 private:
  std::clock_t start_time_;
};

}  // namespace psz

#endif  //__CLOCK_H