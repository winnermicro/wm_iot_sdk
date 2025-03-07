#include <stdio.h>
#include <stdlib.h>
#include "wm_utils.h"
#include "yaml.h"
#include "wm_error.h"
#include "wm_diskio.h"
#include "ff.h"

#define LOG_TAG "yaml_emmc_example"
#include "wm_log.h"

#define WM_EMMC_YAML_FILE             "test.yml"
#define WM_EMMC_YAML_READ_SIZE        1500
#define FATFS_FORMAT_WORK_BUFFER_SIZE (4096) /**< Size of the work buffer used for formatting the file system */

typedef struct {
    char *db_server;
    char *db_pass;
    char *db_user;
    char *rail_user;
    char *rail_pass;
} conf_t;

const static char *yaml_file = "db_server: \"localhost\"\n"
                               "db_password: \"test\"\n"
                               "db_username: \"abc\"\n"
                               "national_rail_username: test\n"
                               "national_rail_password: wibble\n"
                               "languages:\n"
                               "  - Ruby\n"
                               "  - Perl\n"
                               "  - Python\n"
                               "websites:\n"
                               "  YAML: yaml.org\n"
                               "  Ruby: ruby-lang.org\n"
                               "  Python: python.org\n"
                               "  Perl: use.perl.org";

static int write_conf(const char *file_path)
{
    FRESULT res;
    FIL fil;
    uint32_t br;

    res = f_open(&fil, file_path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        wm_log_error("Failed to open file for writing: %d", res);
        return WM_ERR_FAILED;
    }

    res = f_write(&fil, yaml_file, strlen(yaml_file), &br);
    if (res != FR_OK) {
        wm_log_error("f_write file: %d", res);
    }

    f_close(&fil);

    return WM_ERR_SUCCESS;
}

static conf_t *read_conf(const char *file_path)
{
    int state = 0;
    char **datap;
    char *tk;
    yaml_parser_t parser;
    yaml_token_t token;
    FIL fil;
    FRESULT res;

    res = f_open(&fil, file_path, FA_READ);
    if (res != FR_OK) {
        wm_log_error("Failed to open file for writing: %d", res);
        return NULL;
    }

    conf_t *conf = malloc(sizeof(*conf));
    if (!conf) {
        return NULL;
    }
    memset(conf, 0, sizeof(*conf));

    if (!yaml_parser_initialize(&parser)) {
        wm_log_error("Failed to initialize parser!\n");
        free(conf);
        return NULL;
    }
    yaml_parser_set_input_file(&parser, (FILE *)&fil);

    do {
        if (!yaml_parser_scan(&parser, &token)) {
            wm_log_error("scan error %d %s\n", parser.error, parser.problem);
            break;
        }

        switch (token.type) {
            case YAML_KEY_TOKEN:
                state = 0;
                break;
            case YAML_VALUE_TOKEN:
                state = 1;
                break;
            case YAML_SCALAR_TOKEN:
                tk = (char *)(token.data.scalar.value);
                if (state == 0) {
                    /* It's safe to not use strncmp as one string is a literal */
                    if (!strcmp(tk, "db_server")) {
                        datap = &conf->db_server;
                    } else if (!strcmp(tk, "db_password")) {
                        datap = &conf->db_pass;
                    } else if (!strcmp(tk, "db_username")) {
                        datap = &conf->db_user;
                    } else if (!strcmp(tk, "national_rail_username")) {
                        datap = &conf->rail_user;
                    } else if (!strcmp(tk, "national_rail_password")) {
                        datap = &conf->rail_pass;
                    } else {
                        datap = NULL;
                    }
                } else {
                    if (datap)
                        *datap = strdup(tk);
                }
                break;
            default:
                break;
        }
        if (token.type != YAML_STREAM_END_TOKEN)
            yaml_token_delete(&token);
    } while (token.type != YAML_STREAM_END_TOKEN);

    yaml_token_delete(&token);
    yaml_parser_delete(&parser);

    f_close(&fil);
    return conf;
}

static void example_parser(char *file_path)
{
    if (WM_ERR_SUCCESS == write_conf(file_path)) {
        conf_t *conf = read_conf(file_path);

        if (conf) {
            wm_log_info("%s, %s\n", conf->db_server, conf->db_user);

            if (conf->db_server)
                free(conf->db_server);
            if (conf->db_pass)
                free(conf->db_pass);
            if (conf->db_user)
                free(conf->db_user);
            if (conf->rail_user)
                free(conf->rail_user);
            if (conf->rail_pass)
                free(conf->rail_pass);
            free(conf);
        }
    }
    return;
}

static FRESULT format_filesystem(const char *path)
{
    FRESULT res;
    MKFS_PARM opt;
    void *work;

    uint32_t len = FATFS_FORMAT_WORK_BUFFER_SIZE; // Size of work buffer
    opt.fmt      = FM_ANY;                        // Format type, FM_ANY means auto-select
    opt.n_fat    = 1;                             // Number of FAT tables
    opt.align    = 0;                             // Data area alignment, 0 means no alignment
    opt.n_root   = 0;                             // Number of root directory entries for FAT12/16, 0 means use maximum
    opt.au_size  = 0;                             // Cluster size, 0 means use default size

    work = malloc(len);
    if (work == NULL) {
        wm_log_error("Failed to allocate work buffer.\n");
        return FR_INT_ERR;
    }

    res = f_mkfs(path, &opt, work, len);
    free(work);
    work = NULL;

    if (res != FR_OK) {
        wm_log_error("File system formatting failed with error: %d", res);
    }

    return res;
}

static FRESULT mount_filesystem(FATFS *fs, char *path)
{
    FRESULT res = f_mount(fs, path, 1);
    if (res == FR_NO_FILESYSTEM) {
        wm_log_info("File system not found. Formatting...");
        res = format_filesystem(path);
        if (res == FR_OK) {
            res = f_mount(fs, path, 1);
        }
    }
    if (res != FR_OK) {
        wm_log_error("Failed to mount file system.");
    } else {
        wm_log_info("File system mount success, fs_type: %d", fs->fs_type);
    }

    return res;
}

int main(void)
{
    FRESULT res;
    FATFS *fs          = NULL;
    char path[8]       = { 0 };
    char full_path[32] = { 0 };

    /* Allocate memory for FATFS structure that needs to be valid for the entire filesystem lifetime */
    fs = (FATFS *)malloc(sizeof(FATFS));
    memset(fs, 0, sizeof(FATFS));

    /**
     * @note If CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE=y is configured in menuconfig,
     *       use the following volume ID format to support Windows-like drive letters (C:/D:/E:):
     *       sprintf(path, "%s:", CONFIG_FATFS_SDMMC_DISK_FF_VOLUME_STRS);
     */
    sprintf(path, "%d:", WM_DISKIO_DRIVER_NUM_SDMMC);
    res = mount_filesystem(fs, path);
    if (res != FR_OK) {
        goto exit1;
    }
    sprintf(full_path, "%s%s", path, WM_EMMC_YAML_FILE);

    example_parser(full_path);
exit1:
    free(fs);
    fs = NULL;

    return res;
}
