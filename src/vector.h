#ifndef __PSZ_VECTOR_H
#define __PSZ_VECTOR_H

#include <ostream>
#include <vector>

// stream helper for printing vectors
template <typename T>
std::ostream& operator<<(std::ostream& ostr, const std::vector<T>& vec) {
  if (vec.empty()) {
    ostr << "{}";
    return ostr;
  }

  typename std::vector<T>::const_iterator iter = vec.begin();
  ostr << "{" << *iter;
  ++iter;
  while (iter != vec.end()) {
    ostr << ',' << *iter;
    ++iter;
  }
  ostr << "}";
  return ostr;
}

#endif  // __PSZ_VECTOR_H
