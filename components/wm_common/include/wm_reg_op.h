#ifndef __WM_REG_OP_H__
#define __WM_REG_OP_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_BIT(x)      (1UL << (x))
#define WM_BIT_MASK(n) (WM_BIT(n) - 1UL)

/**
 * @defgroup REG_APIs REG APIs
 * @brief Register operate APIs
 */

/**
 * @addtogroup REG_APIs
 * @{
 */

/**
 * @brief          This function is used to write value to register or memory
 *
 * @param[in]      _r    address to be written
 * @param[in]      _v    value to write
 *
 * @return         None
 *
 * @note           None
 */
#define WM_REG32_WRITE(_r, _v)               \
    do {                                     \
        (*(volatile uint32_t *)(_r)) = (_v); \
    } while (0)

/**
 * @brief          This function is used to read value from register or memory
 *
 * @param[in]      _r     address to read
 *
 * @retval         value  read from register or memory
 *
 * @note           None
 */
#define WM_REG32_READ(_r)        ({ (*(volatile uint32_t *)(_r)); })

/**
 * @brief          This function is used to read a single bit from address
 *
 * @param[in]      _r    32-bit aligned byte address where the bit exists
 * @param[in]      _b    Bit position
 *
 * @retval         The value of the requested bit
 */
#define WM_REG32_GET_BIT(_r, _b) ({ (*(volatile uint32_t *)(_r) & (_b)); })

/**
 * @brief          This function is used to write specified bit is 1
 *
 * @param[in]      _r    32-bit aligned byte address where the bit exists.
 * @param[in]      _b    Bit position.
 *
 * @return         None
 */
#define WM_REG32_SET_BIT(_r, _b)                                          \
    do {                                                                  \
        *(volatile uint32_t *)(_r) = (*(volatile uint32_t *)(_r)) | (_b); \
    } while (0)

/**
 * @brief          This function is used to write specified bit is 0
 *
 * @param[in]      _r    32-bit aligned byte address where the bit exists.
 * @param[in]      _b    Bit position.
 *
 * @return         None
 */
#define WM_REG32_CLR_BIT(_r, _b)                                             \
    do {                                                                     \
        *(volatile uint32_t *)(_r) = (*(volatile uint32_t *)(_r)) & (~(_b)); \
    } while (0)

/**
 * @brief          This function is used to write specified bits by mask
 *
 * @param[in]      _r    32-bit aligned byte address where the bit exists.
 * @param[in]      _b    Bits position.
 * @param[in]      _m    mask.
 *
 * @return         None
 */
#define WM_REG32_SET_BITS(_r, _b, _m)                                                      \
    do {                                                                                   \
        *(volatile uint32_t *)(_r) = (*(volatile uint32_t *)(_r) & ~(_m)) | ((_b) & (_m)); \
    } while (0)

/* TODO: remove debug message in future */
#if 0
#include <stdio.h>
#define WM_LL_LOG_INF   printf
#define WM_LL_MARKER(a) WM_LL_LOG_INF("LL:%04d->%-16s 0x%08x\n", __LINE__, #a, a)
#else
#define WM_LL_MARKER(a)
#endif

/**
 * @brief Macro to force a 32-bit read, modify, then write on a peripheral register
 *
 * @note When the assembly uses st.b and st.h to read and write peripheral registers
 *  via the APB BUS, the values will be coerced into integers, updating and replacing
 *  the entire 32-bit register value, which can result in the original value being incorrectly overwritten.
 */
#define WM_LL_REG_FILED_COPY_WRTIE(container, member, member_val) \
    {                                                             \
        uint32_t temp_val = container.val;                        \
        typeof(container) s_temp;                                 \
        s_temp.val    = temp_val;                                 \
        s_temp.member = (member_val);                             \
        container.val = s_temp.val;                               \
        s_temp.val    = container.val;                            \
        WM_LL_MARKER(s_temp.member);                              \
        WM_LL_MARKER(s_temp.val);                                 \
    }

/**
 * @brief Macro to force a 32-bit read on a peripheral register
 *
 * @note When the assembly uses st.b and st.h to read and write peripheral registers
 *  via the APB BUS, the values will be coerced into integers, updating and replacing
 *  the entire 32-bit register value, which can result in the original value being incorrectly overwritten.
 */
#define WM_LL_REG_FILED_COPY_READ(container, member) \
    ({                                               \
        uint32_t reg_val = container.val;            \
        typeof(container) s_temp;                    \
        s_temp.val = reg_val;                        \
        s_temp.member;                               \
    })

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_REG_OP_H__ */
