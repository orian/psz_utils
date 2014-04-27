#include "string_split.h"

namespace psz {

std::vector<std::string> SplitString(const std::string& str,
                                     const std::string& sep) {
  std::vector<std::string> results;
  size_t prev = 0;
  size_t pos = str.find(sep);
  while (pos != std::string::npos) {
    results.push_back(str.substr(prev, pos - prev));
    prev = pos + 1;
    pos = str.find(sep, prev);
  }
  results.push_back(str.substr(prev, str.length() - prev));
  return results;
}

std::vector<unsigned int> SplitStringAndParse(const std::string& str,
                                              const std::string& sep,
                                              int shift) {
  std::vector<unsigned int> result;
  for (const auto& val : SplitString(str, sep)) {
    if (val.empty()) {
      continue;
    }
    unsigned int val_u = std::stoul(val);
    // CHECK_LE(-shift, val_u);
    result.push_back(val_u + shift);
  }
  return result;
}

}  // namespace psz
