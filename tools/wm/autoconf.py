#!/usr/bin/python
import os
import re
import argparse

#["keyword1", "keyword2", "kconfig_item", "component_name"]
component_list = [["cJSON",                 "cJSON/cJSON",          "COMPONENT_CJSON_ENABLED",              "cjson"],
                  ["bluetooth",             "bluetooth/",           "COMPONENT_BT_ENABLED",                 "bluetooth"],
                  ["coap",                  "coap3/coap",           "COMPONENT_COAP_ENABLED",               "coap"],
                  ["lwip",                  "lwip/",                "COMPONENT_LWIP_ENABLED",               "lwip"],
                  ["mbedtls",               "mbedtls/",             "COMPONENT_MBEDTLS_ENABLED",            "mbedtls"],
                  ["wm_mqtt_client",        "MQTT",                 "COMPONENT_MQTT_ENABLED",               "mqtt"],
                  ["sys/types.h",           "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["errno.h",               "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["fcntl.h",               "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["mqueue.h",              "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["pthread.h",             "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["sched.h",               "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["semaphore.h",           "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["signal.h",              "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["time.h",                "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["unistd.h",              "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["utils.h",               "FreeRTOS_POSIX",       "COMPONENT_POSIX_ENABLED",              "posix"],
                  ["wm_atcmd",              "wm_atcmd.h",           "COMPONENT_WM_ATCMD_ENABLED",           "wm_atcmd"],
                  ["wm_event",              "wm_event.h",           "COMPONENT_EVENT_ENABLED",              "wm_event"],
                  ["wm_http_client",        "HTTPClient",           "COMPONENT_HTTP_CLIENT_ENABLED",        "wm_httpclient"],
                  ["wm_http2_client",       "nghttp2",              "COMPONENT_HTTP_CLIENT_ENABLED",        "wm_httpclient"],
                  ["wm_nm",                 "wm_nm_",               "COMPONENT_NET_MANAGER_ENABLED",        "wm_net_manager"],
                  ["wm_nvs",                "wm_nvs.h",             "COMPONENT_NVS_ENABLED",                "wm_nvs"],
                  ["wm_ota",                "wm_ota_",              "COMPONENT_OTA_ENABLED",                "wm_ota"],
                  ["wm_wifi",               "wm_wifi_",             "COMPONENT_WIFI_ENABLED",               "wm_wifi"],
                  ["wm_pm",                 "wm_pm.h",              "COMPONENT_PM_ENABLED",                 "wm_pm"],
                  ["ff.h",                  "wm_diskio",            "COMPONENT_FATFS_ENABLED",              "fatfs"],
                  ["yaml",                  "yaml.h",               "COMPONENT_YAML_ENABLED",               "yaml"]]

parser = argparse.ArgumentParser(usage=None, description='wmsdk autoconf tool', prog="autoconf.py")
parser.add_argument("-p", "--project", help="project path", default="")
parser.add_argument("-s", "--sdk", help="sdk path", default="")
parser.add_argument("-c", "--config", help="kconfig file", default="")

def find_files(directory, pattern):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(pattern):
                yield os.path.join(root, file)

def extract_includes(file_path):
    includes = set()
    try:
        with open(file_path, 'r', encoding='ISO-8859-1') as file:
            for line in file:
                match = re.search(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', line)
                if match:
                    includes.add(match.group(1))
    except UnicodeDecodeError:
        pass
    return includes

def collect_includes(directory, extensions=('.c', '.h')):
    includes = set()
    for file_path in find_files(directory, extensions):
        includes.update(extract_includes(file_path))
    return includes

if __name__ == '__main__':
    parser_args = parser.parse_args()

    if not parser_args.project or not os.path.exists(parser_args.project.strip()):
        print("no available project path")
        exit(0)
    elif not parser_args.config or not os.path.exists(parser_args.config.strip()):
        print("no available kconfig file")
        exit(0)

    configs = set()
    includes = collect_includes(parser_args.project.strip())
    for include in includes:
        for i in range(len(component_list)):
            if component_list[i][0] in include or component_list[i][1] in include:
                configs.add(component_list[i][2])
                if parser_args.sdk and not os.path.exists(os.path.join(parser_args.sdk.strip(), "components", component_list[i][3])):
                    print("-- Autoconf: maybe component %s not exist" % component_list[i][2])
    if configs:
        project_config = parser_args.project.strip() + "/prj.config"
        if os.path.exists(project_config):
            with open(project_config, 'r', encoding='utf-8') as config_file:
                lines = config_file.readlines()
                temp_configs = set()
                lines_str = "".join(lines)
                for config in configs:
                    if "# CONFIG_" + config + " is not set" in lines_str or "CONFIG_" + config + "=n" in lines_str:
                        temp_configs.add(config)
                configs = configs - temp_configs
        with open(parser_args.config.strip(), 'r', encoding='utf-8') as config_file:
            lines = config_file.readlines()
            temp_configs = set()
            lines_str = "".join(lines)
            for config in configs:
                if "CONFIG_" + config + "=y" in lines_str:
                    temp_configs.add(config)
            configs = configs - temp_configs
            for config in configs:
                try:
                    lines.remove("# CONFIG_" + config + " is not set\n")
                except ValueError:
                    pass
            new_lines = "".join(lines)
            if configs:
                new_lines = new_lines + "\n"
                for config in configs:
                    new_lines = new_lines + "CONFIG_" + config + "=y\n"
            if new_lines != "".join(lines):
                with open(parser_args.config.strip(), 'w+') as config_file:
                    config_file.write(new_lines)
                    #print("-- Autoconf: config file has been updated")
                    exit(1)
    exit(0)
