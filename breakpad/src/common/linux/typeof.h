#ifndef COMMON_LINUX_TYPEOF_H
#define COMMON_LINUX_TYPEOF_H

#if __cplusplus >= 201103L
#include <type_traits>
#define TYPEOF(x) std::remove_reference<decltype(x)>::type
#else
#define TYPEOF(x) typeof(x)
#endif

#endif
