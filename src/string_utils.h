#ifndef __PSZ_STRING_UTILS_H
#define __PSZ_STRING_UTILS_H

#include <string>

namespace psz {

bool EndsWith(std::string const &value, std::string const &suffix);

bool StartsWith(std::string const &value, std::string const &prefix);

}  // namespace psz

#endif  // __PSZ_STRING_UTILS_H