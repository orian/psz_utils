#ifndef __PSZ_STRING_JOIN_H
#define __PSZ_STRING_JOIN_H

#include <sstream>

template <typename T1>
std::string StrCat(const T1& t1) {
  std::ostringstream str;
  str << t1;
  return str.str();
}

template <typename T1, typename T2>
std::string StrCat(const T1& t1, const T2& t2) {
  std::ostringstream str;
  str << t1 << t2;
  return str.str();
}

template <typename T1, typename T2, typename T3>
std::string StrCat(const T1& t1, const T2& t2, const T3& t3) {
  std::ostringstream str;
  str << t1 << t2 << t3;
  return str.str();
}

template <typename T1, typename T2, typename T3, typename T4>
std::string StrCat(const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
  std::ostringstream str;
  str << t1 << t2 << t3 << t4;
  return str.str();
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
std::string StrCat(const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                   const T5& t5) {
  std::ostringstream str;
  str << t1 << t2 << t3 << t4 << t5;
  return str.str();
}

#endif  // __PSZ_STRING_JOIN_H
