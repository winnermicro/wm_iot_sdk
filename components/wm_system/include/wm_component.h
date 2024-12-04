#ifndef __WM_COMPONENT_H__
#define __WM_COMPONENT_H__

#ifdef __cplusplus
extern "C" {
#endif

struct wm_component_init {
    int (*init_func)(void);
};

#define wm_component_init0_start()                                                                  \
    ({                                                                                              \
        static char __start[0] __attribute__((used, aligned(4), section(".wm_component_init0_0"))); \
        (struct wm_component_init *)&__start;                                                       \
    })

#define wm_component_init0_end()                                                                  \
    ({                                                                                            \
        static char __end[0] __attribute__((used, aligned(4), section(".wm_component_init0_2"))); \
        (struct wm_component_init *)&__end;                                                       \
    })

#define wm_components_init0()                                                                               \
    struct wm_component_init *__component;                                                                  \
    for (__component = wm_component_init0_start(); __component < wm_component_init0_end(); __component++) { \
        __component->init_func();                                                                           \
    }

#define wm_component_init0_declare(phase, function)               \
    struct wm_component_init __component_init0_##phase_##function \
        __attribute__((used, aligned(4), section(".wm_component_init0_" phase)))

#define wm_component_init_start()                                                                  \
    ({                                                                                             \
        static char __start[0] __attribute__((used, aligned(4), section(".wm_component_init_0"))); \
        (struct wm_component_init *)&__start;                                                      \
    })

#define wm_component_init_end()                                                                  \
    ({                                                                                           \
        static char __end[0] __attribute__((used, aligned(4), section(".wm_component_init_5"))); \
        (struct wm_component_init *)&__end;                                                      \
    })

#define wm_components_init()                                                                              \
    struct wm_component_init *__component;                                                                \
    for (__component = wm_component_init_start(); __component < wm_component_init_end(); __component++) { \
        __component->init_func();                                                                         \
    }

#define wm_component_init_declare(phase, function)               \
    struct wm_component_init __component_init_##phase_##function \
        __attribute__((used, aligned(4), section(".wm_component_init_" phase)))

/* Running sequence:
    WM_COMPONEN_INIT_0
    WM_COMPONEN_INIT_1
    WM_COMPONEN_INIT_2
    WM_COMPONEN_INIT_3
    WM_COMPONEN_INIT_4

   so:
    0: init
    1: boot
    2: system
    3: general
    4: user
 */
#define WM_COMPONEN_INIT_0(component_init_function)              \
    wm_component_init0_declare("1", component_init_function) = { \
        .init_func = component_init_function,                    \
    }

#define WM_COMPONEN_INIT_1(component_init_function)             \
    wm_component_init_declare("1", component_init_function) = { \
        .init_func = component_init_function,                   \
    }

#define WM_COMPONEN_INIT_2(component_init_function)             \
    wm_component_init_declare("2", component_init_function) = { \
        .init_func = component_init_function,                   \
    }

#define WM_COMPONEN_INIT_3(component_init_function)             \
    wm_component_init_declare("3", component_init_function) = { \
        .init_func = component_init_function,                   \
    }

#define WM_COMPONEN_INIT_4(component_init_function)             \
    wm_component_init_declare("4", component_init_function) = { \
        .init_func = component_init_function,                   \
    }

#ifdef __cplusplus
}
#endif

#endif /* __WM_COMPONENT_H__ */
