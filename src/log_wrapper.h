#ifndef __LOG_WRAPPER_H
#define __LOG_WRAPPER_H

#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <streambuf>

///////////////////////////////////////////////////////////////////////////////
// Nullstream, discards everything what printed to it.
///////////////////////////////////////////////////////////////////////////////
template <class cT, class traits = std::char_traits<cT>>
class basic_nullbuf : public std::basic_streambuf<cT, traits> {
  typename traits::int_type overflow(typename traits::int_type c) {
    return traits::not_eof(c);  // indicate success
  }
};

template <class cT, class traits = std::char_traits<cT>>
class basic_onullstream : public std::basic_ostream<cT, traits> {
 public:
  basic_onullstream()
      : std::basic_ios<cT, traits>(&m_sbuf),
        std::basic_ostream<cT, traits>(&m_sbuf) {
    // std::basic_ios<cT, traits>::init(&m_sbuf);
  }
  static basic_onullstream<cT, traits>& Get() {
    if (!singleton_) {
      singleton_.reset(new basic_onullstream<cT, traits>());
    }
    return *singleton_.get();
  }

 private:
  basic_nullbuf<cT, traits> m_sbuf;
  static std::unique_ptr<basic_onullstream<cT, traits>> singleton_;
};

template <class cT, class traits>
std::unique_ptr<basic_onullstream<cT, traits>>
    basic_onullstream<cT, traits>::singleton_(nullptr);

typedef basic_onullstream<char> onullstream;
typedef basic_onullstream<wchar_t> wonullstream;

///////////////////////////////////////////////////////////////////////////////
// LOG and LOG_IF, simulating google LOG
///////////////////////////////////////////////////////////////////////////////

#define TRACE trace
#define DEBUG debug
#define INFO info
#define WARNING warning
#define ERROR error
#define FATAL fatal

#define LOG(lvl) BOOST_LOG_TRIVIAL(lvl) << __FILE__ << ':' << __LINE__ << ": "

#define LOG_IF(lvl, condition) \
  if (condition) LOG(lvl)

///////////////////////////////////////////////////////////////////////////////
// CHECKs mimicing Google Logging
///////////////////////////////////////////////////////////////////////////////


// #define CHECK(condition)                           \
//   if (!condition) {                                \
//     LOG(FATAL) << "Check failed: " #condition " "; \
//     exit(EXIT_FAILURE);                            \
//   }

#define CHECK(condition) \
  LOG_IF(FATAL, !(condition)) << "Check failed: " #condition " "

// A helper class for formatting "expr (V1 vs. V2)" in a CHECK_XX
// statement.  See MakeCheckOpString for sample usage.  Other
// approaches were considered: use of a template method (e.g.,
// base::BuildCheckOpString(exprtext, base::Print<T1>, &v1,
// base::Print<T2>, &v2), however this approach has complications
// related to volatile arguments and function-pointer arguments).
class CheckOpMessageBuilder {
 public:
  // Inserts "exprtext" and " (" to the stream.
  explicit CheckOpMessageBuilder(const char* exprtext)
      : stream_(new std::ostringstream) {
    *stream_ << exprtext << " (";
  }
  // Deletes "stream_".
  ~CheckOpMessageBuilder() { delete stream_; }
  // For inserting the first variable.
  std::ostream* ForVar1() { return stream_; }
  // For inserting the second variable (adds an intermediate " vs. ").
  std::ostream* ForVar2() {
    *stream_ << " vs. ";
    return stream_;
  }
  // Get the result (inserts the closing ")").
  std::string NewString() {
    *stream_ << ")";
    return stream_->str();
  }

 private:
  std::ostringstream* stream_;
};

// This formats a value for a failing CHECK_XX statement.  Ordinarily,
// it uses the definition for operator<<, with a few special cases below.
template <typename T>
inline void MakeCheckOpValueString(std::ostream* os, const T& v) {
  (*os) << v;
}

template <>
void MakeCheckOpValueString(std::ostream* os, const char& v);

template <>
void MakeCheckOpValueString(std::ostream* os, const signed char& v);

template <>
void MakeCheckOpValueString(std::ostream* os, const unsigned char& v);

// GCC can be told that a certain branch is not likely to be taken (for
// instance, a CHECK failure), and use that information in static analysis.
// Giving it this information can help it optimize for the common case in
// the absence of better information (ie. -fprofile-arcs).
//
#ifndef GOOGLE_PREDICT_BRANCH_NOT_TAKEN
#if ac_cv_have___builtin_expect
#define GOOGLE_PREDICT_BRANCH_NOT_TAKEN(x) (__builtin_expect(x, 0))
#define GOOGLE_PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define GOOGLE_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define GOOGLE_PREDICT_BRANCH_NOT_TAKEN(x) x
#define GOOGLE_PREDICT_FALSE(x) x
#define GOOGLE_PREDICT_TRUE(x) x
#endif
#endif

template <typename T1, typename T2>
std::string MakeCheckOpString(const T1& v1, const T2& v2,
                              const char* exprtext) {
  CheckOpMessageBuilder comb(exprtext);
  MakeCheckOpValueString(comb.ForVar1(), v1);
  MakeCheckOpValueString(comb.ForVar2(), v2);
  return comb.NewString();
}

// Helper functions for CHECK_OP macro.
// The (int, int) specialization works around the issue that the compiler
// will not instantiate the template version of the function on values of
// unnamed enum type - see comment below.
#define DEFINE_CHECK_OP_IMPL(name, op)                                  \
  template <typename T1, typename T2>                                   \
  inline std::string name##Impl(const T1& v1, const T2& v2,             \
                                const char* exprtext) {                 \
    if (v1 op v2)                                                       \
      return NULL;                                                      \
    else                                                                \
      return MakeCheckOpString(v1, v2, exprtext);                       \
  }                                                                     \
  inline std::string name##Impl(int v1, int v2, const char* exprtext) { \
    return name##Impl<int, int>(v1, v2, exprtext);                      \
  }

// We use the full name Check_EQ, Check_NE, etc. in case the file including
// base/logging.h provides its own #defines for the simpler names EQ, NE, etc.
// This happens if, for example, those are used as token names in a
// yacc grammar.
DEFINE_CHECK_OP_IMPL(Check_EQ, == )  // Compilation error with CHECK_EQ(NULL, x)
DEFINE_CHECK_OP_IMPL(Check_NE, != )  // Use CHECK(x == NULL) instead.
DEFINE_CHECK_OP_IMPL(Check_LE, <= )
DEFINE_CHECK_OP_IMPL(Check_LT, < )
DEFINE_CHECK_OP_IMPL(Check_GE, >= )
DEFINE_CHECK_OP_IMPL(Check_GT, > )
#undef DEFINE_CHECK_OP_IMPL

// A container for a string pointer which can be evaluated to a bool -
// true iff the pointer is NULL.
struct CheckOpString {
  CheckOpString(std::string* str) : str_(str) {}
  ~CheckOpString() {
    if (str_ != NULL) {
      delete str_;
    }
  }
  // No destructor: if str_ is non-NULL, we're about to LOG(FATAL),
  // so there's no point in cleaning up str_.
  operator bool() const {
    return GOOGLE_PREDICT_BRANCH_NOT_TAKEN(str_ != NULL);
  }
  std::string* str_;
};

// Function is overloaded for integral types to allow static const
// integrals declared in classes and not defined to be used as arguments to
// CHECK* macros. It's not encouraged though.
template <class T>
inline const T& GetReferenceableValue(const T& t) {
  return t;
}
inline char GetReferenceableValue(char t) { return t; }
inline unsigned char GetReferenceableValue(unsigned char t) { return t; }
inline signed char GetReferenceableValue(signed char t) { return t; }
inline short GetReferenceableValue(short t) { return t; }
inline unsigned short GetReferenceableValue(unsigned short t) { return t; }
inline int GetReferenceableValue(int t) { return t; }
inline unsigned int GetReferenceableValue(unsigned int t) { return t; }
inline long GetReferenceableValue(long t) { return t; }
inline unsigned long GetReferenceableValue(unsigned long t) { return t; }
inline long long GetReferenceableValue(long long t) { return t; }
inline unsigned long long GetReferenceableValue(unsigned long long t) {
  return t;
}

// In optimized mode, use CheckOpString to hint to compiler that
// the while condition is unlikely.
#define CHECK_OP_LOG(name, op, val1, val2)                             \
  LOG_IF(FATAL, !(val1 op val2))                                       \
      << "CHECK" #name                                                 \
         " failed: " << MakeCheckOpString(GetReferenceableValue(val1), \
                                          GetReferenceableValue(val2), \
                                          #val1 " " #op " " #val2)

#define CHECK_OP(name, op, val1, val2) CHECK_OP_LOG(name, op, val1, val2)
//#define CHECK_OP(name, op, val1, val2) CHECK((val1)op(val2))

#define CHECK_EQ(val1, val2) CHECK_OP(_EQ, ==, val1, val2)
#define CHECK_NE(val1, val2) CHECK_OP(_NE, !=, val1, val2)
#define CHECK_LE(val1, val2) CHECK_OP(_LE, <=, val1, val2)
#define CHECK_LT(val1, val2) CHECK_OP(_LT, <, val1, val2)
#define CHECK_GE(val1, val2) CHECK_OP(_GE, >=, val1, val2)
#define CHECK_GT(val1, val2) CHECK_OP(_GT, >, val1, val2)

//#undef CHECK_OP

#endif  // __LOG_WRAPPER_H
