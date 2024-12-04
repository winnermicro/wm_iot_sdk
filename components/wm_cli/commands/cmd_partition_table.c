#include "wm_types.h"
#include "wm_cli.h"
#include "wm_partition_table.h"

static void cmd_partition(int argc, char *argv[])
{
    wm_partition_table_print();
}
WM_CLI_CMD_DEFINE(partition, cmd_partition, partition table cmd, partition -- show partition table);
