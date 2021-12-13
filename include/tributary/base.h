#pragma once


#ifdef WIN32
#ifdef T_EXPORTS
#define T_EXPORT __declspec(dllexport)
#else
#define T_EXPORT __declspec(dllimport)
#endif
#else
#define T_EXPORT __attribute__((visibility("default")))
#endif
