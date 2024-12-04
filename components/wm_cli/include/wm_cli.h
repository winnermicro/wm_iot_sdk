#ifndef __WM_CLI_H__
#define __WM_CLI_H__

#include "wmsdk_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_COMPONENT_CLI_ENABLED

#define WM_CLI_CMD_SECTION      ".wm_cli_cmd"

#define wm_cli_cmd_declare(cmd) struct wm_cli_cmd __cli_cmd_##cmd __attribute__((used, section(WM_CLI_CMD_SECTION)))

/**
  * @brief command handler.
  *
  * @param argc  number of input parameters.
  * @param argv  input parameters.
  *
  */
typedef void (*wm_cli_handler)(int argc, char *argv[]);

struct wm_cli_cmd {
    char *name;              /**< command name */
    wm_cli_handler function; /**< command handler */
    char *desc;              /**< command description */
    char *usg;               /**< command detailed usage */
};

/**
 * @defgroup WM_CLI_APIs WM CLI APIs
 * @brief WinnerMicro CLI APIs
 */

/**
 * @addtogroup WM_CLI_APIs
 * @{
 */

/**
 * @brief  define an cli command
 *
 * @param[in] cmd           cmd name
 * @param[in] handler       cmd handler funtion
 * @param[in] description   cmd description
 * @param[in] usage         cmd usage
 *
 * @return  None
 */
#if CONFIG_CLI_ENABLE_LONG_USAGE
#define WM_CLI_CMD_DEFINE(cmd, handler, description, usage) \
    wm_cli_cmd_declare(cmd) = {                             \
        .name     = #cmd,                                   \
        .function = handler,                                \
        .desc     = #description,                           \
        .usg      = #usage,                                 \
    }
#else
#define WM_CLI_CMD_DEFINE(cmd, handler, description, usage) \
    wm_cli_cmd_declare(cmd) = {                             \
        .name     = #cmd,                                   \
        .function = handler,                                \
        .desc     = #description,                           \
        .usg      = NULL,                                   \
    }
#endif

/**
  * @brief  Initialize cli
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_cli_init(void);

/**
 * @brief  output cli information
 *
 * @param[in] fmt           string format list
 * @param[in] ...           variable parameters
 *
 * @return                  the number of bytes transmitted
 */
int wm_cli_printf(const char *fmt, ...);

/**
 * @}
 */

#else /* CONFIG_COMPONENT_CLI_ENABLED */

#define WM_CLI_CMD_DEFINE(cmd, handler, description, usage)

#define wm_cli_printf(fmt, ...)

#endif /* CONFIG_COMPONENT_CLI_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __WM_CLI_H__ */
