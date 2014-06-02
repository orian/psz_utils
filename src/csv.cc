#include "csv.h"

#include "log_wrapper.h"

#include <limits>

namespace psz {

CsvRow::CsvRow() : scanned_(0) {}

std::string const& CsvRow::operator[](std::size_t index) const {
  CHECK_LT(index, scanned_);
  return m_data_[index];
}

std::size_t CsvRow::size() const { return scanned_; }

void CsvRow::readNextRow(std::istream& str) {
  scanned_ = 0;
  std::string line;
  std::getline(str, line);

  std::stringstream lineStream(line);
  std::string cell;

  //  m_data_.clear();
  std::vector<std::string>::iterator iter = m_data_.begin();
  std::vector<std::string>::iterator end_iter = m_data_.end();
  while (iter != end_iter && std::getline(lineStream, cell, ',')) {
    *iter = cell;
    ++scanned_;
    ++iter;
  }
  if (iter == end_iter) {
    while (std::getline(lineStream, cell, ',')) {
      ++scanned_;
      m_data_.push_back(cell);
    }
  }
  // Assuming scanned_ should be the same for all rows, resize is not needed.
  // m_data_.resize(scanned_);
}

std::istream& operator>>(std::istream& str, CsvRow& data) {
  data.readNextRow(str);
  return str;
}

CsvReader::CsvReader(const std::string& filepath)
    : has_next_(true), file_(filepath) {
  CHECK(file_);
  CHECK(file_.is_open());
}

CsvReader::~CsvReader() { file_.close(); }

const CsvRow& CsvReader::Get() const {
  CHECK(has_next_);
  return row_;
}

bool CsvReader::Next() {
  if (has_next_) {
    has_next_ = file_ >> row_;
  }
  return has_next_;
}

bool CsvReader::End() const { return !has_next_; }

CsvReaderWithLimits::CsvReaderWithLimits(const std::string& filepath,
                                         uint skip_rows, uint limit_rows)
    : CsvReader(filepath),
      skip_rows_(skip_rows),
      limit_rows_(limit_rows > 0 ? skip_rows + limit_rows
                                 : std::numeric_limits<uint>::max()),
      current_row_(0) {
  //  LOG(INFO) << skip_rows_ << ", num_rows:" << limit_rows_;
}

bool CsvReaderWithLimits::Next() {
  if (current_row_ >= limit_rows_) {
    return false;
  }
  while (current_row_ < skip_rows_) {
    // // // LOG_EVERY_N(INFO, 50000)
    // << "skipping rows: " << std::min(skip_rows_, current_row_ + 50000)
    // << " of " << skip_rows_ << " to skip.";
    if (!CsvReader::Next()) {
      return false;
    }
    ++current_row_;
  }
  ++current_row_;
  return CsvReader::Next();
}

bool CsvReaderWithLimits::End() const {
  return CsvReader::End() || current_row_ >= limit_rows_;
}

std::unique_ptr<CsvReader> CsvFactory(const std::string& path,
                                      const uint skip_rows,
                                      const uint limit_rows) {
  if (skip_rows > 0 || limit_rows > 0) {
    return std::unique_ptr<CsvReader>(
        new CsvReaderWithLimits(path, skip_rows, limit_rows));
  } else {
    return std::unique_ptr<CsvReader>(new CsvReader(path));
  }
}

}  // namespace psz
