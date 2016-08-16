#include "string_utils.h"

namespace psz {

bool EndsWith(std::string const &value, std::string const &suffix) {
  return suffix.size() <= value.size() &&
         std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

bool StartsWith(std::string const &value, std::string const &prefix) {
  return prefix.size() <= value.size() &&
         std::equal(prefix.begin(), prefix.end(), value.begin());
}

}  // namespace psz
