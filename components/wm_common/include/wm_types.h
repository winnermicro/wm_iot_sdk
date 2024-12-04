#ifndef __WM_TYPES_H__
#define __WM_TYPES_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WM_INVALID_HANDLE ((void *)0)

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __WM_TYPES_H__ */
