#include <stdio.h>
#include <stdint.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_partition_table.h"

#define LOG_TAG "exam_pt"
#include "wm_log.h"

int main(void)
{
    int ret;
    wm_partition_item_t partition;

    wm_log_info("This example succeeds in running.");

    /*init partition table*/
    ret = wm_partition_table_init();
    if (ret != WM_ERR_SUCCESS) {
        wm_log_info("Partition table init failed.");
    }

    /*find nvs partition*/
    ret = wm_partition_table_find("nvs", &partition);
    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("Get nvs OK,offset=0x%08x,size=0x%x\n", partition.offset, partition.size);
    } else {
        wm_log_info("Get nvs failed,ret=%d\n", ret);
    }

    /*print partitions*/
    wm_partition_table_print();

    return 0;
}
