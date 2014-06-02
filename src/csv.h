#ifndef __CSV_H
#define __CSV_H

#include "macros.h"

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>

namespace psz {

class CsvRow {
 public:
  CsvRow();
  std::string const& operator[](std::size_t index) const;
  std::size_t size() const;
  void readNextRow(std::istream& str);

 private:
  std::vector<std::string> m_data_;
  uint scanned_;
};

std::istream& operator>>(std::istream& str, CsvRow& data);

class CsvReader {
 public:
  explicit CsvReader(const std::string& filepath);
  virtual ~CsvReader();
  virtual const CsvRow& Get() const;
  virtual bool Next();
  virtual bool End() const;

 private:
  bool has_next_;
  std::ifstream file_;
  CsvRow row_;

  DISALLOW_COPY_AND_ASSIGN(CsvReader);
};

class CsvReaderWithLimits : public CsvReader {
 public:
  explicit CsvReaderWithLimits(const std::string& filepath, uint skip_rows,
                               uint limit_rows);
  virtual bool Next();
  virtual bool End() const;

 private:
  const uint skip_rows_;
  const uint limit_rows_;
  uint current_row_;

  DISALLOW_COPY_AND_ASSIGN(CsvReaderWithLimits);
};

std::unique_ptr<CsvReader> CsvFactory(const std::string& path,
                                      const uint skip_rows,
                                      const uint limit_rows);

}  // namespace psz

#endif  // __CSV_H
