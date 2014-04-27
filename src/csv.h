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

namespace utils {

class CSVRow {
 public:
  std::string const& operator[](std::size_t index) const;
  std::size_t size() const;
  void readNextRow(std::istream& str);

 private:
  std::vector<std::string> m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data);

class CSVReader {
 public:
  explicit CSVReader(const std::string& filepath);
  virtual ~CSVReader();
  virtual const CSVRow& Get() const;
  virtual bool Next();
  virtual bool End() const;

 private:
  bool has_next_;
  std::ifstream file_;
  CSVRow row_;

  DISALLOW_COPY_AND_ASSIGN(CSVReader);
};

class CSVReaderWithLimits : public CSVReader {
 public:
  explicit CSVReaderWithLimits(const std::string& filepath, uint skip_rows,
                               uint limit_rows);
  virtual bool Next();
  virtual bool End() const;

 private:
  const uint skip_rows_;
  const uint limit_rows_;
  uint current_row_;

  DISALLOW_COPY_AND_ASSIGN(CSVReaderWithLimits);
};

std::unique_ptr<CSVReader> CSVFactory(const std::string& path,
                                      const uint skip_rows,
                                      const uint limit_rows);

}  // namespace utils

#endif  // __CSV_H
