#!/usr/bin/python3
import os
import re
import sys
import json
import time
import shutil
import hashlib
import argparse
import platform
import threading
import subprocess
import webbrowser

from colorama import Fore, Back, Style, init
init(autoreset=True)

python_exec = "python"
sdk_env_name = "WM_IOT_SDK_PATH"
soc_list = ["w800", "w801s", "w802", "w803", "w805", "w806"]

sdk_path = os.path.abspath(sys.path[0] + "/../../")
try:
    if os.environ[sdk_env_name] and os.path.exists(os.environ[sdk_env_name]):
        sdk_path = os.environ[sdk_env_name]
except Exception:
    print(Fore.YELLOW + "environment variable \"WM_IOT_SDK_PATH\" maybe set incorrectly.")

tools_dir = sdk_path + "/tools/wm"
if os.path.exists(tools_dir):
    sys.path.insert(1, tools_dir)
    from flash import parser as flash_parser
    from monitor import parser as monitor_parser
    from ramdump import parser as ramdump_parser

cmd_help = '''
set-soc   : set chip type
devconfig : open device config pannel
menuconfig: open menuconfig pannel
build     : start compile project
clean     : clean build files
flash     : burn firmware to flash
monitor   : display serial output
distclean : clean all build files
guiconfig : open guiconfig window
saveconfig: save current config as default
ramdump   : save the ramdump file
debug     : start the gdb analysis ramdump file
'''

cmd_list = ["set-soc", "devconfig", "menuconfig", "build", "clean", "flash", "monitor", "distclean", "config", "guiconfig", "saveconfig", "guiflash", "gdb", "ramdump", "debug", "coverage", "lib"]

project_parser = argparse.ArgumentParser(usage="""[%(prog)s] is build tool for winnermicro sdk, it implements function to config, build and flash sdk image.""",
                                         prog="wmsdk",
                                         formatter_class=argparse.RawTextHelpFormatter,
                                         parents=[flash_parser, monitor_parser, ramdump_parser],
                                         conflict_handler='resolve'
                                         )

project_parser.add_argument("commands",
                            help=cmd_help,
                            nargs='+'
                            )

project_parser.add_argument('-g', '--generator', default="", nargs='+', help="project type to generate, supported type on your platform see `cmake --help`")

project_parser.add_argument('--list-socs',
                            help='list the currently supported SOCs',
                            action="store_true",
                            default=False
                            )

project_parser.add_argument('--disable-autoconf',
                            help='disable the automatic derivation feature',
                            action="store_true",
                            default=False
                            )

project_parser.add_argument('--verbose',
                            help='show verbose when compile',
                            action="store_true",
                            default=False
                            )

project_parser.add_argument('--preprocess',
                            help='preprocess c/c++ code without compiling',
                            action="store_true",
                            default=False
                            )

project_parser.add_argument('--time',
                            help='show elapsed time after compile',
                            action="store_true",
                            default=False
                            )

project_args = project_parser.parse_args()

skip_next_command = False
for command in project_args.commands:
    if skip_next_command:
        skip_next_command = False
        continue
    if command in cmd_list:
        if command == "set-soc":
            skip_next_command = True
        continue
    else:
        project_parser.print_help()
        sys.exit(1)

#os.environ["MENUCONFIG_STYLE"] = "aquatic selection=fg:white,bg:yellow"

project_path = os.getcwd()
project_name = ""
project_cmake_path = os.path.join(project_path, "CMakeLists.txt")
project_cmake_content = ""
binary_path = os.path.join(os.path.abspath(os.getcwd()), "build")
sdk_config_file = os.path.join(binary_path, "config", "wmsdk.config")

if not os.path.exists("CMakeLists.txt") or  not os.path.exists("main"):
    print(Fore.RED + "please run me at project folder!")
    sys.exit(1)

with open(project_cmake_path) as f:
    project_cmake_content = f.read()
match = re.findall(r"set\(PROJECT_NAME (.*)\)", project_cmake_content)
if len(match) != 0:
    project_name = match[0]
    if("project_dir_name" in project_name):
        project_name = os.path.basename(project_path)
#    print("-- project name: {}".format(project_name))
if project_name == "":
    print(Fore.RED + "[ERROR] Can not find project name in {}, not set(PROJECT_NAME {})".format(project_cmake_path, "${project_name}"))
    sys.exit(1)

image_path = os.path.join(binary_path, project_name + ".fls")
image_partition_table_path = os.path.join(binary_path, "partition_table", "partition_table.img")
image_bootloader_path = os.path.join(binary_path, "bootloader", "bootloader.img")
image_app_path = os.path.join(binary_path, project_name + ".img")
custom_image_path = os.path.join(binary_path, "custom_bin")
elf_path = os.path.join(binary_path, project_name + ".elf")
ramdump_path = os.path.join(binary_path, project_name + ".ramdump")

if "Darwin" == platform.system():
    gdb_path = "csky-unknown-elf-gdb"
else:
    gdb_path = "csky-elfabiv2-gdb"
gdb_script = os.path.join(sdk_path, "tools/wm/w800.gdb")

if "Windows" == platform.system():
    python_exec = "python"
else:
    python_exec = sys.executable

os.chdir(project_path)

def do_kconfig(show):
    tool_path = os.path.join(sdk_path, "tools/wm/kconfig.py")
    if not os.path.exists(tool_path):
        print(Fore.RED + "[ERROR] kconfig tool not found:", tool_path)
        sys.exit(1)
    cmd = [python_exec, tool_path, "--kconfig", os.path.join(sdk_path, "tools/wm/Kconfig")]
    config_default = os.path.join(project_path, "prj.config")
    if os.path.exists(config_default):
        cmd.extend(["--defaults", config_default])
    if show:
        cmd.extend(["--menuconfig", "True", "--env", "SDK_PATH=" + sdk_path, "--env", "PROJECT_PATH=" + project_path, "--env", "HOST_OS_TYPE=" + platform.system()])
    else:
        cmd.extend(["--menuconfig", "False", "--env", "SDK_PATH=" + sdk_path, "--env", "PROJECT_PATH=" + project_path, "--env", "HOST_OS_TYPE=" + platform.system()])
    cmd.extend(["--output", "config", sdk_config_file])
    cmd.extend(["--output", "cmake", os.path.join(binary_path, "config", "wmsdk_config.cmake")])
    cmd.extend(["--output", "header", os.path.join(binary_path, "config", "wmsdk_config.h")])
    res = subprocess.call(cmd)
    if res != 0:
        sys.exit(1)
    return

def show_guiconfig():
    tool_path = os.path.join(sdk_path, "tools/wm/kconfig.py")
    if not os.path.exists(tool_path):
        print(Fore.RED + "[ERROR] kconfig tool not found:", tool_path)
        sys.exit(1)
    cmd = [python_exec, tool_path, "--kconfig", os.path.join(sdk_path, "tools/wm/Kconfig")]
    config_default = os.path.join(project_path, "prj.config")
    if os.path.exists(config_default):
        cmd.extend(["--defaults", config_default])
    cmd.extend(["--guiconfig", "True", "--env", "SDK_PATH=" + sdk_path, "--env", "PROJECT_PATH=" + project_path, "--env", "HOST_OS_TYPE=" + platform.system()])
    cmd.extend(["--output", "config", sdk_config_file])
    cmd.extend(["--output", "cmake", os.path.join(binary_path, "config", "wmsdk_config.cmake")])
    cmd.extend(["--output", "header", os.path.join(binary_path, "config", "wmsdk_config.h")])
    res = subprocess.call(cmd)
    if res != 0:
        sys.exit(1)
    return

def save_defconfig():
    current_kconfig = os.path.join(project_path, sdk_config_file)
    if not os.path.exists(current_kconfig):
        print(Fore.RED + "[ERROR] current config not exist")
        sys.exit(1)
    tool_path = os.path.join(sdk_path, "tools/wm/kconfig.py")
    if not os.path.exists(tool_path):
        print(Fore.RED + "[ERROR] kconfig tool not found:", tool_path)
        sys.exit(1)
    cmd = [python_exec, tool_path, "--kconfig", os.path.join(sdk_path, "tools/wm/Kconfig")]
    config_default = os.path.join(project_path, "prj.config")
    if os.path.exists(config_default):
        cmd.extend(["--defaults", config_default])
    cmd.extend(["--minimal", config_default, "--env", "SDK_PATH=" + sdk_path, "--env", "PROJECT_PATH=" + project_path, "--env", "HOST_OS_TYPE=" + platform.system()])
    cmd.extend(["--output", "config", sdk_config_file])
    res = subprocess.call(cmd)
    if res != 0:
        print(Fore.RED + "save proj.config failed")
        sys.exit(1)

    current_devconfig = os.path.join(project_path, "build", "device_table", "device_table.toml")
    devconfig_default = os.path.join(project_path, "device_table.toml")
    if os.path.exists(current_devconfig):
        shutil.copy(current_devconfig, devconfig_default)

    print(Fore.GREEN + "current config saved")

def save_soc_changed(old_type, new_type):
    soc_changed = os.path.join(binary_path, "soc_type_changed")
    with open(soc_changed, "w") as file:
        file.write("(%s -> %s)" % (old_type, new_type))

def check_kconfig():
    soc_changed = os.path.join(binary_path, "soc_type_changed")
    if os.path.exists(soc_changed):
        changed = ""
        with open(soc_changed, "r") as file:
            changed = file.readline()
            changed = changed.strip()
        print(Fore.YELLOW + "Chip type of project has changed%s, \nplease delete the build folder and then reset the chip type." % changed)
        sys.exit(1)

def check_autoconf():
    # autoconf check
    if not project_args.disable_autoconf and os.path.exists(sdk_config_file):
        res = subprocess.call([python_exec, tools_dir + "/autoconf.py", "--project", project_path, "--config", sdk_config_file, "--sdk", sdk_path])
        if res == 1:
            do_kconfig(0)

def get_chip_type():
    # chip type check
    if not os.path.exists(sdk_config_file):
        return ""

    with open(sdk_config_file, "r", encoding="utf-8") as file:
        for line in file:
            line = line.strip()
            if line.startswith("#"):
                continue
            if line.startswith("CONFIG_CHIP_NAME"):
                match = re.search(r'="([^"]*)"', line)
                if match:
                    return match.group(1)

    return ""

def do_guiflash():
    if not os.path.exists(image_path):
        print(Fore.RED + "image file not exist")
        sys.exit(1)
    flash_image_list = ""
    if os.path.exists(custom_image_path):
        bin_files = os.listdir(custom_image_path)
        for bin_file in bin_files:
            if bin_file.endswith(".img"):
                custom_image = os.path.join(custom_image_path, bin_file)
                flash_image_list = flash_image_list + ' ' + custom_image + ' '
    flash_image_list = flash_image_list + ' ' + image_partition_table_path + ' ' + image_bootloader_path + ' ' + image_app_path
    guiflash_path = os.path.join(sdk_path, "tools/wm/guiflash.py")
    res = subprocess.call(python_exec + " " + guiflash_path + " " + flash_image_list, shell=True)
    if res != 0:
        print(Fore.RED + "guiflash device failed")

def do_config():
    if project_args.generator:
        if os.path.exists("build"):
            shutil.rmtree("build")
        os.mkdir("build")
        os.chdir("build")
        os.mkdir("config")
        do_kconfig(0)
        check_autoconf()
        generators = ""
        for generator in project_args.generator:
            generators = generators + generator + " "
        if generators[-1] == " ":
            generators = generators[:-1]
        res = subprocess.call(["cmake", "-G", generators, ".."])
        if res != 0:
            sys.exit(1)
        else:
            print(Fore.GREEN + "generate project '%s' complete" % generators)

def do_set_soc(soc_type):
    if not soc_type in soc_list:
        print(Fore.RED + f"unsupported soc type, currently supported soc is {soc_list}")
        sys.exit(1)
    conf_str = "CONFIG_BUILD_TYPE_" + soc_type.upper() + "=y"
    if os.path.exists(sdk_config_file):
        with open(sdk_config_file, 'r', encoding='utf-8') as file:
            for line in file:
                if conf_str in line:
                    print(Fore.YELLOW + f"the current soc type is already {soc_type}")
                    sys.exit(0)
        print("remove the old soc configuration")
        shutil.rmtree("build")
    os.makedirs("build/config")
    with open(sdk_config_file, "w", encoding="utf-8") as file:
        file.write(conf_str + "\n")
        file.write("\n")
    do_kconfig(0)
    print(Fore.GREEN + "generate the new soc configuration")

def check_project_build_path():
    cmake_install_file = "cmake_install.cmake"
    prj_path_prefix = "# Install script for directory: "
    if os.path.exists(cmake_install_file):
        with open(cmake_install_file, 'r', encoding='utf-8') as cmake_install:
            lines = cmake_install.readlines()
            for line in lines:
                if line.startswith(prj_path_prefix):
                    index = line.find(prj_path_prefix)
                    if index != -1:
                        prj_path = line[index + len(prj_path_prefix):]
                        prj_path = prj_path.strip()
                        rules_path = os.path.normpath(prj_path)
                        if project_path != rules_path:
                            print(Fore.RED + "\nThe current rules path may be different the project path.")
                            print("current rules   path: " + rules_path)
                            print("current project path: " + project_path)
                            print(Fore.CYAN + "\nIf you copied or moved the project from somewhere, \nplease remove the build folder and re-run the build command.\n")
                            sys.exit(2)
                    break

def check_partition_table():
    with open("build.ninja", 'r', encoding='utf-8') as build_ninja:
        found = False
        line = build_ninja.readlines()
        for i in line:
            if found:
                start_marker = "tools/wm/partition.py"
                start_pos = i.find(start_marker)
                if start_pos == -1:
                    return
                end_pos = i.find("\"", start_pos + len(start_marker))
                if end_pos == -1:
                    return
                pt_conf = i[start_pos + len(start_marker):end_pos].strip()
                break
            if "CMakeFiles/show_pt: CUSTOM_COMMAND" in i:
                found = True
    if not found:
        return
    pt_hash = hashlib.md5()
    with open(pt_conf, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            pt_hash.update(chunk)
    pt_hash = pt_hash.hexdigest()
    pt_hash_file = os.path.join(binary_path, "partition_table", "partition_table.hash")
    if os.path.exists(pt_hash_file):
        with open(pt_hash_file, 'r', encoding='utf-8') as hash_file:
            line = hash_file.readline()
            if line != pt_hash:
                build_cmd = ["cmake", "--build", ".", "--target", "update_pt"]
                if project_args.verbose:
                    build_cmd.append("--verbose")
                res = subprocess.call(build_cmd)
                if res != 0:
                    print(Fore.RED + "update partition table failed")
                    sys.exit(1)
                with open(pt_hash_file, "w") as f:
                    f.write(pt_hash)
    else:
        with open(pt_hash_file, "w") as f:
            f.write(pt_hash)

def do_build():
    if not os.path.exists("build"):
        os.mkdir("build")
    else:
        check_kconfig()
    os.chdir("build")
    if not os.path.exists("device_table"):
        os.mkdir("device_table")
    if not os.path.exists("device_table/wm_dt_hw.c"):
        with open("device_table/wm_dt_hw.c", "w") as file:
            pass
    if not os.path.exists("config"):
        os.mkdir("config")
    if not os.path.exists(sdk_config_file):
        do_kconfig(0)
    check_autoconf()
    build_cmd = ["cmake", "-G", "Ninja", ".."]
    if not os.path.exists("build.ninja"):
        if project_args.verbose:
            build_cmd.append("-DUSE_CMAKE_BUILD_DEBUG=ON")
        if project_args.preprocess:
            build_cmd.append("-DUSE_CMAKE_PRE_PROCESS=ON")
        res = subprocess.call(build_cmd)
        if res != 0:
            print(Fore.RED + "update compile rules failed")
            sys.exit(1)
    # check is have build and build is from other move to here
    check_project_build_path()
    check_partition_table()
    # touch project_elf_src.c
    project_src = os.path.join(binary_path, "project", "project_elf_src.c")
    if os.path.exists(project_src):
        with open(project_src, 'a'):
            os.utime(project_src, None)
    build_cmd = ["cmake", "--build", ".", "--target", "update_ld", "--target", "show_pt", "--target", "update_dt", "--target", "all"]
    if project_args.verbose:
        build_cmd.append("--verbose")
    res = subprocess.call(build_cmd)
    if res != 0:
        print(Fore.RED + "WM IoT SDK build failed")
        sys.exit(1)
    #else:
    #    print(Fore.GREEN + "WM IoT SDK build success")

def devconfig_server_task():
    time.sleep(1.5)
    webbrowser.open("http://127.0.0.1:31118/wmdt/index")

def do_devconfig():
    if not os.path.exists("build"):
        os.mkdir("build")
    else:
        check_kconfig()
    os.chdir("build")
    if not os.path.exists("config"):
        os.mkdir("config")
    os.chdir("config")
    if not os.path.exists(sdk_config_file):
        do_kconfig(0)
    check_autoconf()

    chip_type = "w800"
    use_toml = False
    with open(sdk_config_file, "r", encoding="utf-8") as file:
        for line in file:
            line = line.strip()

            if line.startswith("#"):
                if line.startswith("# CONFIG_DT_USE_C_STYLE_CONF is not set"):
                    use_toml = True
                continue

            if line.startswith("CONFIG_CHIP_NAME"):
                match = re.search(r'="([^"]*)"', line)
                if match:
                    chip_type = match.group(1)

    if not use_toml:
        print(Fore.YELLOW + "\nCurrent using C file configuration mode. \n\n")
        print(Fore.CYAN + "If you want to Use the device config web page, \ntry again after disable the \"Use c style configuration\" option in the menuconfig.\n")
        sys.exit(0)

    if not os.path.exists(binary_path + "/device_table"):
        os.mkdir(binary_path + "/device_table")

    default_toml_file = sdk_path + "/components/wm_dt/config/" + chip_type + "/device_table.toml"
    project_toml_file = project_path + "/device_table.toml"
    temp_toml_file = binary_path + "/device_table/device_table.toml"
    if os.path.exists(temp_toml_file):
        pass
    elif os.path.exists(project_toml_file):
        shutil.copy(project_toml_file, temp_toml_file)
    else:
        shutil.copy(default_toml_file, temp_toml_file)
    toml_file = temp_toml_file
    print("current toml file: " + toml_file)
    print(Fore.CYAN + "Wait for the browser to open automatically, \npress ctrl+c to terminate the configuration when closing.")
    thread = threading.Thread(target=devconfig_server_task)
    thread.daemon = True
    thread.start()
    res = subprocess.call([python_exec, sdk_path + "/tools/devconf/devconf.py", "-t", toml_file, "-c", sdk_config_file, "-a", "5"])
    if res != 0:
        print(Fore.RED + "exec device config failed")
        sys.exit(1)

def do_flash():
    new_argvs = [python_exec, os.path.join(sdk_path, "tools/wm/flash.py")]
    new_argvs.extend(sys.argv[2:])
    exclude_custom = False
    if project_args.name:
        for i in range(len(new_argvs)):
            if "-n" == new_argvs[i] or "--name" == new_argvs[i]:
                if "app" == new_argvs[i + 1]:
                    new_argvs[i + 1] = project_name
                    break
                elif "custom" == new_argvs[i + 1]:
                    exclude_custom = True
                    break
    if not project_args.list:
        if os.path.exists(custom_image_path) and not exclude_custom:
            bin_files = os.listdir(custom_image_path)
            for bin_file in bin_files:
                if bin_file.endswith(".img"):
                    custom_image = os.path.join(custom_image_path, bin_file)
                    new_argvs.append('-i')
                    new_argvs.append(custom_image)
        new_argvs.append('-i')
        new_argvs.append(image_partition_table_path)
        new_argvs.append('-i')
        new_argvs.append(image_bootloader_path)
        new_argvs.append('-i')
        new_argvs.append(image_app_path)
    res = subprocess.call(new_argvs)
    if res != 0:
        print(Fore.RED + "flash device failed")

def do_monitor():
    new_argvs = [python_exec, os.path.join(sdk_path, "tools/wm/monitor.py")]
    new_argvs.extend(sys.argv[2:])
    try:
        res = subprocess.call(new_argvs)
        if res != 0:
            print(Fore.RED + "monitor device failed")
    except KeyboardInterrupt:
        pass

def do_ramdump():
    if not os.path.exists("build"):
        os.mkdir("build")
    new_argvs = [python_exec, os.path.join(sdk_path, "tools/wm/ramdump.py")]
    new_argvs.extend(sys.argv[2:])
    new_argvs.append('-o')
    new_argvs.append(ramdump_path)
    res = subprocess.call(new_argvs)
    if res != 0:
        print(Fore.RED + "export ramdump failed")

def do_debug():
    stub_port = "2013"
    if "Darwin" == platform.system():
        gdb_stub = os.path.join(sdk_path, "tools/wm/mac_gdb.stub")
        res = os.popen(gdb_stub + " " + elf_path + " " + ramdump_path + " " + stub_port)
        if not res:
            print(Fore.RED + "exec gdb stub failed")
            sys.exit(1)
    else:
        if "Windows" == platform.system():
            gdb_stub = os.path.join(sdk_path, "tools/wm/win_gdb.stub")
        else:
            gdb_stub = os.path.join(sdk_path, "tools/wm/linux_gdb.stub")
        res = subprocess.Popen(gdb_stub + " " + elf_path + " " + ramdump_path + " " + stub_port, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        try:
            res.wait(timeout=0.5)
        except subprocess.TimeoutExpired:
            pass
        else:
            print(Fore.RED + "exec gdb stub failed")
            sys.exit(1)
    debug_script = os.path.join(binary_path, "ramdump.gdb")
    script_data = ""
    conn_info = "#target remote :1025"
    with open(gdb_script, "r", encoding="utf-8") as gf:
        for line in gf:
            if conn_info in line:
                line = line.replace(conn_info, "target remote :" + stub_port)
            script_data += line
    with open(debug_script, "w", encoding="utf-8") as gf:
        gf.write(script_data)
    res = subprocess.call(gdb_path + " -q " + elf_path + " -x " + debug_script, shell=True)
    if res != 0:
        print(Fore.RED + "exec gdb failed")

def do_code_coverage():
    if not os.path.exists("build"):
        print(Fore.YELLOW + "need to compile the project first!")
        sys.exit(1)
    if not os.path.exists("build/code_coverage"):
        os.mkdir("build/code_coverage")
    os.chdir("build/code_coverage")
    print(Fore.CYAN + "wait generate code coverage......")
    res = subprocess.call(["gcovr", "-r", sdk_path, "--html", "--html-details", "-o", "index.html", "--gcov-executable", "csky-elfabiv2-gcov", "--html-title", "WMSDK Code Coverage Report"])
    if res != 0:
        sys.exit(1)
    else:
        print(Fore.GREEN + "generate code coverage complete")
        webbrowser.open("index.html")

def do_lib():
    res = subprocess.call(["cmake", "--build", ".", "--target", "lib"])
    if res != 0:
        sys.exit(1)

try:
    for command in project_args.commands:
        if command == "set-soc":
            if len(project_args.commands) != 2:
                if project_args.list_socs:
                    print(Fore.RED + f"currently supported soc is {soc_list}")
                    sys.exit(0)
                else:
                    print(Fore.RED + "the soc type parameter is missing")
                    sys.exit(1)
            do_set_soc(project_args.commands[1])
            break
        elif command == "build":
            if project_args.time:
                time_start = time.time()
            do_build()
            if project_args.time:
                time_end = time.time()
                print(Fore.CYAN + "elapsed time %.2fs" %(time_end-time_start))
        elif command == "clean":
            if os.path.exists("build"):
                os.chdir("build")
                p = subprocess.Popen(["cmake", "--build", ".", "--target", "clean"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                output, err = p.communicate("")
                res = p.returncode
                if res == 0:
                    print(output.decode())
            print(Fore.GREEN + "clean complete")
        elif command == "distclean":
            if os.path.exists("build"):
                shutil.rmtree("build")
            print(Fore.GREEN + "clean complete")
        elif command == "devconfig":
            do_devconfig()
        elif command == "menuconfig":
            if not os.path.exists("build"):
                os.mkdir("build")
            os.chdir("build")
            if not os.path.exists("config"):
                os.mkdir("config")
            old_type = get_chip_type()
            do_kconfig(1)
            new_type = get_chip_type()
            if old_type and new_type != old_type:
                save_soc_changed(old_type, new_type)
        elif command == "guiconfig":
            if not os.path.exists("build"):
                os.mkdir("build")
            os.chdir("build")
            if not os.path.exists("config"):
                os.mkdir("config")
            old_type = get_chip_type()
            show_guiconfig()
            new_type = get_chip_type()
            if old_type and new_type != old_type:
                save_soc_changed(old_type, new_type)
        elif command == "saveconfig":
            save_defconfig()
        elif command == "guiflash":
            do_guiflash()
        elif command == "config":
            do_config()
        elif command == "flash":
            if not project_args.list:
                do_build()
            do_flash()
        elif command == "monitor":
            do_monitor()
        elif command == "gdb":
            res = subprocess.call(gdb_path + " -q " + elf_path + " -x " + gdb_script, shell=True)
            if res != 0:
                print(Fore.RED + "exec gdb failed")
        elif command == "ramdump":
            do_ramdump()
        elif command == "debug":
            do_debug()
        elif command == "coverage":
            do_code_coverage()
        elif command == "lib":
            do_build()
            do_lib()
except KeyboardInterrupt:
    pass
except OSError:
    print(Fore.RED + "operation failed")