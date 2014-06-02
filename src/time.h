#ifndef __PSZ_TIME_H
#define __PSZ_TIME_H

constexpr unsigned int SEC_IN_MIN = 60;
constexpr unsigned int MIN_IN_HOUR = 60;
constexpr unsigned int SEC_IN_HOUR = MIN_IN_HOUR * SEC_IN_MIN;
constexpr unsigned int HOUR_IN_DAY = 24;
constexpr unsigned int MIN_IN_DAY = HOUR_IN_DAY * MIN_IN_HOUR;
constexpr unsigned int SEC_IN_DAY = MIN_IN_DAY * SEC_IN_MIN;
constexpr unsigned int DAYS_IN_WEEK = 7;

#endif  // __PSZ_TIME_H
