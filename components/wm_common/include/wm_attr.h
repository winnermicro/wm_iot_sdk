#ifndef __WM_ATTR_H__
#define __WM_ATTR_H__

#include "wmsdk_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ATTRIBUTE_ISR       __attribute__((isr))
#define ATTRIBUTE_IRQRAM    __attribute__((section("IRQINRAM")))
#define ATTRIBUTE_IRAM      __attribute__((section("TEXTINIRAM")))
#define ATTRIBUTE_SHAREDRAM __attribute__((section("SHAREDRAM")))
#define ATTRIBUTE_PSRAM     __attribute__((section("INPSRAM")))
#define ATTRIBUTE_ALIGN4    __attribute__((aligned(4)))
#define ATTRIBUTE_WEAK      __attribute__((weak))

#ifdef CONFIG_ATTRIBUTE_INLINE_ALWAYS
#define ATTRIBUTE_INLINE static inline __attribute__((always_inline))
#else
#define ATTRIBUTE_INLINE static inline
#endif

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATTRS_H__ */
