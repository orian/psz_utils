#include "csv.h"

#include "log_wrapper.h"

#include <limits>

namespace utils {

std::string const& CSVRow::operator[](std::size_t index) const {
  // CHECK_LT(index, m_data.size());
  return m_data[index];
}

std::size_t CSVRow::size() const { return m_data.size(); }

void CSVRow::readNextRow(std::istream& str) {
  std::string line;
  std::getline(str, line);

  std::stringstream lineStream(line);
  std::string cell;

  m_data.clear();
  while (std::getline(lineStream, cell, ',')) {
    m_data.push_back(cell);
  }
}

std::istream& operator>>(std::istream& str, CSVRow& data) {
  data.readNextRow(str);
  return str;
}

CSVReader::CSVReader(const std::string& filepath)
    : has_next_(true), file_(filepath) {
  // CHECK(file_);
  // CHECK(file_.is_open());
}

CSVReader::~CSVReader() {}

const CSVRow& CSVReader::Get() const {
  // CHECK(has_next_);
  return row_;
}

bool CSVReader::Next() {
  if (has_next_) {
    // // LOG(INFO) << "reading row";
    has_next_ = file_ >> row_;
  }
  return has_next_;
}

bool CSVReader::End() const { return !has_next_; }

CSVReaderWithLimits::CSVReaderWithLimits(const std::string& filepath,
                                         uint skip_rows, uint limit_rows)
    : CSVReader(filepath),
      skip_rows_(skip_rows),
      limit_rows_(limit_rows > 0 ? skip_rows + limit_rows
                                 : std::numeric_limits<uint>::max()),
      current_row_(0) {
  // LOG(INFO) << skip_rows_ << ", num_rows:" << limit_rows_;
}

bool CSVReaderWithLimits::Next() {
  if (current_row_ >= limit_rows_) {
    return false;
  }
  while (current_row_ < skip_rows_) {
    // // // LOG_EVERY_N(INFO, 50000)
    // << "skipping rows: " << std::min(skip_rows_, current_row_ + 50000)
    // << " of " << skip_rows_ << " to skip.";
    if (!CSVReader::Next()) {
      return false;
    }
    ++current_row_;
  }
  ++current_row_;
  return CSVReader::Next();
}

bool CSVReaderWithLimits::End() const {
  return CSVReader::End() || current_row_ >= limit_rows_;
}

std::unique_ptr<CSVReader> CSVFactory(const std::string& path,
                                      const uint skip_rows,
                                      const uint limit_rows) {
  if (skip_rows > 0 || limit_rows > 0) {
    return std::unique_ptr<CSVReader>(
        new CSVReaderWithLimits(path, skip_rows, limit_rows));
  } else {
    return std::unique_ptr<CSVReader>(new CSVReader(path));
  }
}

}  // namespace utils
