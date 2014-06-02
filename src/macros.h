#ifndef __PSZ_MACROS_H
#define __PSZ_MACROS_H

#if __cplusplus > 199711L
#define __cplusplus11
#endif

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// copied from:
#define Q_UNUSED(x) (void) x;

#ifdef UNUSED
#elif defined(__GNUC__)
#define UNUSED(x) UNUSED_##x __attribute__((unused))
#elif defined(__LCLINT__)
#define UNUSED(x) /*@unused@*/ x
#else
#define UNUSED(x) x
#endif

#ifdef __cplusplus11
using uint = unsigned int;
using uint32 = unsigned int;
using int32 = signed int;
#else
#define uint unsigned int
#define uint32 unsigned int
#define int32 signed int
#endif

#endif  // __PSZ_MACROS_H
