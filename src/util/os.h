#ifndef NAIVESERV_UTIL_OS_H_
#define NAIVESERV_UTIL_OS_H_

// NOTE by Max: MICROSOFT SUCK!!
#if defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)
#define UTIL_OS_WINDOWS
#ifdef _MSC_VER
#define UTIL_OS_WIN_VS
#endif
#endif

#ifdef UTIL_OS_WIN_VS
// warning about using 'std::localtime_s'
#pragma warning(disable : 4996)
// warnings about integer type casting
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#endif

#endif // NAIVESERV_UTIL_OS_H_
