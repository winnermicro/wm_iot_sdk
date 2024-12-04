import os
import re
import sys
import time
import shutil
import filecmp
import argparse
import platform
import subprocess
import concurrent.futures
from datetime import datetime
from concurrent.futures import ThreadPoolExecutor, as_completed

def find_main_directories(prj_path):
    """
    Recursively walks through the project path to find directories that contain a 'main' subdirectory,
    excluding any paths that include 'build'.

    Parameters:
        prj_path (str): The root path of the project to search through.

    Yields:
        str: The path to the parent directory containing the 'main' directory.
    """
    for root, dirs, files in os.walk(prj_path, topdown=True):
        if 'main' in dirs and 'build' not in root:
            main_dir = os.path.join(root, 'main')
            yield os.path.dirname(main_dir)

def copy_files_with_dirs(src_folder, dst_folder):
    """
    Copies files from a source folder to a destination folder, ensuring that the directory structure is maintained.

    Parameters:
        src_folder (str): The path to the source folder containing the files to copy.
        dst_folder (str): The path to the destination folder where the files will be copied.

    Raises:
        IOError: If there is an error during the file copying process.
    """
    dst_subfolder = os.path.join(dst_folder, src_folder)
    if not os.path.exists(dst_subfolder):
        os.makedirs(dst_subfolder)

    for filename in os.listdir(src_folder):
        file_path = os.path.join(src_folder, filename)
        if os.path.isfile(file_path):
            dst_file_path = os.path.join(dst_subfolder, filename)
            shutil.copy2(file_path, dst_file_path)

def execute_command(command, cwd=None, shell=True):
    """
    Executes a given command in a shell and captures the output, error, and return code.

    Parameters:
        command (str): The command to execute.
        cwd (str): The current working directory to execute the command in.
        shell (bool): Whether to use a shell to execute the command.

    Returns:
        tuple: A tuple containing the stdout, stderr, and return code.
    """
    process = subprocess.Popen(
        command,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=shell
    )
    stdout, stderr = process.communicate()
    return_code = process.returncode

    return stdout, stderr, return_code

def get_modified_files():
    """
    Execute git commands to retrieve a list of modified files between two commits.

    Returns:
        list: A list of file paths that have been modified.
    """
    try:
        modified_files = []
        cmd = "git diff HEAD~1 HEAD --name-only"
        process = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True
        )
        stdout, stderr = process.communicate()
        return_code = process.returncode
        modified_files = stdout.decode('utf-8').strip().split('\n')
        return modified_files
    except subprocess.CalledProcessError as e:
        print("An error occurred while trying to get modified files:", e)

def read_format_ignore_paths(file_path):
    """
    Reads the ignore paths from a configuration file.

    Parameters:
        file_path (str): The path to the configuration file.

    Returns:
        list: A list of paths to ignore.
    """
    ignore_paths = []
    with open(file_path, 'r') as file:
        for line in file:
            line = line.strip()
            if line and not line.startswith('#'):  # Ignore empty lines and comments
                ignore_paths.append(os.path.normpath(line))
    return ignore_paths

def check_format(modified_files, ignore_paths):
    """
    Checks if the modified C or header files have been formatted using clang-format.

    Parameters:
        modified_files (list): A list of file paths that have been modified.
        ignore_paths (list): A list of file paths that have been ignored.

    Returns:
        list: A list of file paths that have not been formatted.
    """
    clang_format_cmd = "clang-format"
    no_format_files = []

    for file_path in modified_files:
        if any(ignore_path in file_path for ignore_path in ignore_paths):
            continue
        path, filename = os.path.split(file_path)
        if filename.endswith(('.c', '.h')):
            if filename.startswith('wm_') or 'examples' in path:
                try :
                    file_path_cf = file_path + ".cf"
                    cmd = f"clang-format -style=file:.clang-format {file_path} > {file_path_cf}"
                    stdout, stderr, return_code = execute_command(cmd)
                    is_formatted = filecmp.cmp(file_path, file_path_cf)
                    if not is_formatted:
                        if os.path.getsize(file_path_cf) > 0:
                            shutil.move(file_path_cf, file_path)
                        no_format_files.append(file_path)
                    os.remove(file_path_cf)
                except:
                    print(f"{file_path}")

    return no_format_files

def check_in_file(filenames, file_to_check):
    """
    Checks if the provided filenames are present within the contents of a specified file.

    Parameters:
        filenames (list): A list of filenames to search for.
        file_to_check (str): The path to the file whose contents will be checked.

    Returns:
        list: A list of filenames that were found within the file's contents.
    """
    found_files = []

    try:
        with open(file_to_check, 'r') as file:
            contents = file.read()
            for filename in filenames:
                if filename in contents:
                    found_files.append(filename)
        return found_files
    except FileNotFoundError:
        print(f"The file {file_to_check} does not exist.")

def extract_config_chip_name(file_path):
    """
    Extracts the value within quotes for the pattern '#define CONFIG_CHIP_NAME "xxx"'
    from the given file.

    Parameters:
    file_path (str): The path to the file to be read.

    Returns:
    str: The content within the quotes, or None if not found or an error occurs.
    """
    # Regular expression pattern to match '#define CONFIG_CHIP_NAME "xxx"'
    pattern = r'#define\s+CONFIG_CHIP_NAME\s+"([^"]+)"'
    
    # Try to open and read the file
    try:
        with open(file_path, 'r') as file:
            for line in file:
                # Use regular expression to search for the pattern
                match = re.search(pattern, line)
                if match:
                    # If a match is found, return the content within the quotes
                    return match.group(1)
    except FileNotFoundError:
        print(f"The file {file_path} was not found.")
    except Exception as e:
        print(f"An error occurred while reading the file: {e}")
    
    # Return None if not found or an error occurred
    return None

def execute_script(main_dir, clean_en, save_path, del_build, verified_build, map_analyzer, ignore_build, modified_files, start_build_time):
    """
    Executes the build script for a given project directory and handles the output and errors.

    Parameters:
        main_dir (str): The main directory where the build script is located.
        clean_en (str): A flag indicating whether a clean build is to be executed.
        save_path (str): The path where the build binaries will be saved.
        del_build (str): A flag indicating whether to delete the build directory after compilation.
        verified_build (str): A flag indicating whether to perform a quick build of the most recently modified files.
        modified_files (list): A list of files that have been modified.
        start_build_time (str): A timestamp used to name the binary save directory.

    Returns:
        tuple: A tuple containing the return code from the build process and a list of new modified files.
    """
    cmd = ""
    new_modified_files = []
    os_name = platform.system()

    # distclean
    if clean_en == "true":
        if os_name == 'Linux':
            cmd = "wm.sh distclean"
        elif os_name == 'Windows':
            cmd = "wm.py distclean"
        stdout, stderr, return_code = execute_command(cmd, cwd = main_dir)
        if return_code == 0:
            print(f"Clean successfully in {main_dir}")
        else:
            print(f"Clean failed in {main_dir}")
            return return_code, modified_files

    # config
    if os_name == 'Linux':
        cmd = "wm.sh config -g Ninja"
    elif os_name == 'Windows':
        cmd = "wm.py config -g Ninja"
    stdout, stderr, return_code = execute_command(cmd, cwd = main_dir)
    if return_code == 0:
        print(f"Config successfully in {main_dir}")
    else:
        print(f"Config failed in {main_dir}")
        return return_code, modified_files

    # When verifying build, the project will exit without compiling without relying on modifying files
    if verified_build == "true":
        found_files = check_in_file(modified_files, main_dir +"/build/build.ninja")
        if not found_files:
            return 0, modified_files
        else:
            for file in modified_files:
                if file not in found_files:
                    new_modified_files.append(file)
            print("found_file", found_files)

    # W800, W801, and W805 do not have touch sensors and can be exited without compilation
    chip_name = extract_config_chip_name(main_dir + "/build/config/wmsdk_config.h")
    if chip_name == "W800" or chip_name == "W801" or chip_name == "W805":
        if "touch" in main_dir:
            return 0, new_modified_files

    # ignore build
    if ignore_build == "true":
        with open(os.path.dirname(os.path.abspath(__file__)) + "/ignore_build.cfg", 'r') as file:
            for line in file:
                stripped_line = line.strip()
                if stripped_line and stripped_line in main_dir:
                    return 0, new_modified_files

    # build
    if os_name == 'Linux':
        cmd = "wm.sh build"
    elif os_name == 'Windows':
        cmd = "wm.py build"
    stdout, stderr, return_code = execute_command(cmd, cwd = main_dir)
    if return_code == 0:
        print(f"Built successfully in {main_dir}")
        if "warning:" in stdout.decode('utf-8'):
            print(f"Built warning in {main_dir}")
            return_code = 1
            print(stdout.decode('utf-8'))
            return return_code, new_modified_files
    else:
        print(f"Built failed in {main_dir}")
        print(stdout.decode('utf-8'))
        return return_code, new_modified_files

    # lib
    if os_name == 'Linux':
        cmd = "wm.sh lib"
    elif os_name == 'Windows':
        cmd = "wm.py lib"
    stdout, stderr, return_code = execute_command(cmd, cwd = main_dir)
    if return_code == 0:
        print(f"lib generated successfully in {main_dir}")
    else:
        print(f"lib generation failed in {main_dir}")
        print(stdout.decode('utf-8'))
        return return_code, new_modified_files

    # map analyzer
    if map_analyzer == "true":
        map_analyzer_exe = "./tools/map_analyzer/map_analyzer.py"
        stack_analyzer_exe = "./tools/stack_analyzer/stack_analyzer.py"
        project_name = os.path.basename(main_dir)
        if os.path.exists(map_analyzer_exe):
            cmd = f"python {map_analyzer_exe} -m {main_dir}/build/{project_name}.map " \
                f"-e {main_dir}/build/{project_name}_map_analysis_lib_results.xlsx " \
                f"-w {main_dir}/build/{project_name}_map_analysis_lib_results.html " \
                f"-o lib -c {main_dir}/build/config/wmsdk_config.h"
            print(cmd)
            stdout, stderr, return_code = execute_command(cmd)
            if return_code == 0:
                print(f"map analysis(lib) successfully in {main_dir}")
            else:
                print(f"map analysis(lib) failed in {main_dir}")
                print(stderr.decode('utf-8'))
                return return_code, new_modified_files

            cmd = f"python {map_analyzer_exe} -m {main_dir}/build/{project_name}.map " \
                f"-e {main_dir}/build/{project_name}_map_analysis_file_results.xlsx " \
                f"-w {main_dir}/build/{project_name}_map_analysis_file_results.html " \
                f"-o file -c {main_dir}/build/config/wmsdk_config.h"
            stdout, stderr, return_code = execute_command(cmd)
            if return_code == 0:
                print(f"map analysis(file) successfully in {main_dir}")
            else:
                print(f"map analysis(file) failed in {main_dir}")
                print(stderr.decode('utf-8'))
                return return_code, new_modified_files

            cmd = f"python {stack_analyzer_exe} -a {main_dir}/build/{project_name}.lst " \
                f"-e {main_dir}/build/{project_name}_stack_analysis_results.xlsx " \
                f"-w {main_dir}/build/{project_name}_stack_analysis_results.html " \
                f"-s 128"
            stdout, stderr, return_code = execute_command(cmd)
            if return_code == 0:
                print(f"stack analysis successfully in {main_dir}")
            else:
                print(f"stack analysis failed in {main_dir}")
                print(stderr.decode('utf-8'))
                return return_code, new_modified_files

    # save build
    if os.path.exists(save_path):
        copy_files_with_dirs(f'{main_dir}/build/', save_path.rstrip('/') + f'/wm_iot_sdk_all_bin_{start_build_time}')

    # del
    if del_build == "true":
        try:
            shutil.rmtree(f"{main_dir}/build/")
        except Exception as e:
            return_code = 1
            print(f"Error deleting directory:{e}")
            return return_code, new_modified_files

    return return_code, new_modified_files

def main(max_workers, clean_en, prj_path, save_path, del_build, verified_build, format_check, map_analyzer, ignore_build):
    """
    Orchestrates the build process for all main directories found in the project path using a thread pool.

    Parameters:
        max_workers (int): The maximum number of threads to use in the thread pool.
        clean_en (str): A flag indicating whether a clean build is to be executed.
        prj_path (str): The root path of the project to search through for main directories.
        save_path (str): The path where the build binaries will be saved.
        del_build (str): A flag indicating whether to delete the build directory after compilation.
        verified_build (str): A flag indicating whether to perform a quick build of the most recently modified files.
    format_check (str): A string flag indicating if code formatting checks should be performed using clang-format.

    Returns:
        int: Number of projects that failed compilation.
    """
    err = 0
    total = 0
    modified_files = []
    start_build_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')

    print(f"Starting compilation, please wait...")
    if verified_build == "true":
        modified_files = get_modified_files()
        print("modified_files:", modified_files)
        if format_check == "true":
            ignore_file_path = os.path.join(os.path.dirname(__file__), '../format/format_ignore.cfg')
            ignore_paths = read_format_ignore_paths(ignore_file_path)
            no_format_files = check_format(modified_files, ignore_paths)
            if no_format_files != []:
                print(f"These files have not been formatted with clang-format[error]:{no_format_files}")
                print("Please ensure clang-format version is 18.1.4, or find other colleagues for help.")
                return 1
        for i in range(len(modified_files)):
            if ".h" in modified_files[i]:
                modified_files[i] = os.path.dirname(modified_files[i])

    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        for main_dir in find_main_directories(prj_path):
            future = executor.submit(execute_script, main_dir, clean_en, save_path, del_build, verified_build, map_analyzer, ignore_build, modified_files, start_build_time)
            result, modified_files = future.result()
            total += 1
            if result != 0:
                err += result
        executor.shutdown(wait=True)
        percentage = (err / total) * 100
        print(f"Build failure rate: {err}/{total} = {percentage:.2f}%")
        return err

if __name__ == "__main__":
    """
    The main function of defining script entry points.
    """
    parser = argparse.ArgumentParser(description='Autobuild script for building projects.')
    parser.add_argument('-w', '--max_works', type=int, default=1,
                        help='Set the maximum number of workers to use for concurrent processing. Default is 1.')
    parser.add_argument('-c', '--clean_en', type=str, default='true',
                        help='Specify whether to enable cleaning of the build environment before the build process. '
                            'Accepted values are "true" or "false". Default is "true".')
    parser.add_argument('-p', '--prj_path', type=str, default="./examples",
                        help='Specify the path to the directory containing example projects. '
                            'Default is the "./examples" directory within the current working directory.')
    parser.add_argument('-s', '--save_path', type=str, default='',
                        help='Specify the directory where the generated binary files will be saved. '
                            'Default is null.')
    parser.add_argument('-d', '--del_build', type=str, default='false',
                        help='Specify whether to delete the build directory after compilation '
                            'Accepted values are "true" or "false". Default is "false".')
    parser.add_argument('-v', '--verified_build', type=str, default='false',
                        help='Specify whether to quickly compile the most recently submitted affected project '
                            'Accepted values are "true" or "false". Default is "false".')
    parser.add_argument('-f', '--format_check', type=str, default='false',
                        help='Specify whether recently submitted files have been formatted '
                            'Accepted values are "true" or "false". Default is "false".')
    parser.add_argument('-m', '--map_analyzer', type=str, default='false',
                        help='Specify the type of map analyzer to use. Set "true" to enable the map analyzer, '
                            'or "false" to disable it. Default is "false".',)
    parser.add_argument('-i', '--ignore_build', type=str, default='true',
                        help='Specify whether to enable ignore compilation. Set "ture" to enable '
                            'or "false" to disable it. Default is "false".',)
    parser.add_argument('-r', '--release_rm', type=str, default='false',
                        help='Specify whether release should execute file deletion. Set "ture" to enable '
                            'or "false" to disable it. Default is "false".',)

    args = parser.parse_args()
    max_workers = args.max_works
    clean_en = args.clean_en.lower()
    prj_path = args.prj_path
    save_path = args.save_path
    del_build = args.del_build.lower()
    verified_build = args.verified_build.lower()
    format_check = args.format_check.lower()
    map_analyzer = args.map_analyzer.lower()
    ignore_build = args.ignore_build.lower()
    release_rm = args.release_rm.lower()

    print('max_workers = ', max_workers)
    print('clean_en = ', clean_en)
    print('prj_path = ', prj_path)
    print('save_path = ', save_path)
    print('del_build = ', del_build)
    print('verified_build = ', verified_build)
    print('format_check = ', format_check)
    print('map_analyzer = ', map_analyzer)
    print('ignore_build = ', ignore_build)
    print('release_rm = ', release_rm)

    if release_rm == "true":
        execute_script("./examples/atcmd", "true", "", "true", "false", "false", "false", {}, 0)
        with open(os.path.dirname(os.path.abspath(__file__)) + "/release_rm.cfg", 'r') as file:
            for line in file:
                stripped_line = line.strip()
                if stripped_line and stripped_line != ".git":
                    if os.path.isfile(stripped_line):
                        os.remove(stripped_line)
                        print(f"{stripped_line} has been delete")
                    elif os.path.isdir(stripped_line):
                        shutil.rmtree(stripped_line)
                        print(f"{stripped_line} has been delete")
                    else:
                        print(f"{stripped_line} does not exist")

    start_time = time.time()
    err = main(max_workers, clean_en, prj_path, save_path, del_build, verified_build, format_check, map_analyzer, ignore_build)
    end_time = time.time()
    print(f"Total build time: {(end_time - start_time) // 1} seconds.")
    sys.exit(err)
