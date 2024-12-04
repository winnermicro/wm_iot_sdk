#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_atcmd.h"

#define LOG_TAG "atcmd"
#include "wm_log.h"

static int wm_cmd_test1(wm_at_type_e type, int argc, char **argv)
{
    if (AT_TYPE_QUERY == type) {
        wm_atcmd_print_result("+TEST1:%d\r\n", 0);

        return WM_AT_RESULT_OK;
    } else if (AT_TYPE_EXEC == type) {
        /* do some things... */

        return WM_AT_RESULT_OK;
    } else {
        return WM_AT_RESULT_FAIL;
    }
}

static int wm_cmd_test2(wm_at_type_e type, int argc, char **argv)
{
    if (AT_TYPE_EXEC == type) {
        if (0 == argc)
            wm_atcmd_print_result("+TEST2:%s\r\n", "no param");
        else if (1 == argc)
            wm_atcmd_print_result("+TEST2:%s\r\n", argv[0]);
        else if (2 == argc)
            wm_atcmd_print_result("+TEST2:%s,%s\r\n", argv[0], argv[1]);
        else
            wm_atcmd_print_result("+TEST2:%s\r\n", "many params");

        return WM_AT_RESULT_OK;
    } else {
        return WM_AT_RESULT_FAIL;
    }
}

WM_ATCMD_DEFINE("AT+TEST1", wm_cmd_test1, NULL);
WM_ATCMD_DEFINE("AT+TEST2", wm_cmd_test2,
                "AT+TEST2?\r\n"
                "AT+TEST2=[arg1][,arg2][,arg3]\r\n"
                "example:AT+TEST2=0\r\n");