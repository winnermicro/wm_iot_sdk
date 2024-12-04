#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_list.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define BUF_LEN 100

class Math {
public:
    int add(int a, int b)
    {
        return a + b;
    }

    double add(double a, double b)
    {
        return a + b;
    }

    char *create_chr()
    {
        wm_log_debug("create instance");
        return ((char *)malloc(sizeof(char)));
    }

    void delete_chr(char *chr)
    {
        wm_log_debug("delete instance");
        if (chr)
            free(chr);
    }
};

static void test_task(void *parameters)
{
    Math math;
    int intResult       = math.add(3, 4);
    double doubleResult = math.add(2.5, 3.7);
    char *buf           = NULL;
    char *chr           = NULL;

    buf = (char *)malloc(BUF_LEN);
    if (!buf) {
        goto exit;
    }
    memset(buf, 0x00, BUF_LEN);

    snprintf(buf, BUF_LEN, "Integer result %d, Double result %lf", intResult, doubleResult);
    wm_log_info("%s", buf);

    chr = math.create_chr();
    if (!chr) {
        wm_log_error("instance create error");
        goto exit;
    }
    *chr = 0xf1;
    wm_log_info("chr %02x", *chr);

exit:
    math.delete_chr(chr);
    if (buf)
        free(buf);
    vTaskDelete(NULL);
}

int main(void)
{
    int err             = 0;
    TaskHandle_t handle = NULL;
    if (pdTRUE != (err = xTaskCreate(test_task, "test", 1024, NULL, 10, &handle))) {
        wm_log_error("test task create fail! %d", err);
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}
