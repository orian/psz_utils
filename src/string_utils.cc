#include "string_utils.h"

namespace psz {

bool EndsWith(std::string const &value, std::string const &ending) {
  return ending.size() <= value.size() && std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

}  // namespace psz
