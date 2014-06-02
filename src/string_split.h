#ifndef __PSZ_STRING_SPLIT_H
#define __PSZ_STRING_SPLIT_H

#include <string>
#include <vector>

namespace psz {

std::vector<std::string> SplitString(const std::string& str,
                                     const std::string& sep);

std::vector<unsigned int> SplitStringAndParse(const std::string& str,
                                              const std::string& sep,
                                              int shift = 0);

}  // namespace psz

#endif  // __PSZ_STRING_SPLIT_H
