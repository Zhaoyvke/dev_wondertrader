#ifndef MARKETDATARECEIVER_GLOBAL_H
#define MARKETDATARECEIVER_GLOBAL_H

#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
#   define XFIN_DECL_EXPORT     __declspec(dllexport)
#   define XFIN_DECL_IMPORT     __declspec(dllimport)
#elif defined(__linux__)
#   define XFIN_DECL_EXPORT     __attribute__((visibility("default")))
#   define XFIN_DECL_IMPORT     __attribute__((visibility("default")))
#else
#   error "Platform not supported"
#   define XFIN_DECL_EXPORT
#   define XFIN_DECL_IMPORT
#endif


#if defined(MARKETDATARECEIVER_LIBRARY)
#  define MARKETDATARECEIVER_EXPORT XFIN_DECL_EXPORT
#else
#  define MARKETDATARECEIVER_EXPORT XFIN_DECL_IMPORT
#endif


#endif // MARKETDATARECEIVER_GLOBAL_H
