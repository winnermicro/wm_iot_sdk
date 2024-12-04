# Automated Build and Code Formatting Check Script

## Prerequisites

- Python 3.x
- Git (for detecting code modifications)
- Clang-format (for code formatting checks)
- Build tools (e.g., wm.sh or wm.py, depending on project requirements)

## Installation

Ensure Python 3.x and the required dependencies are installed. You can check the Python version with the following command:

```bash
python --version
```

Make sure Git and Clang-format are installed and configured in your system's PATH.

## Usage
The script can be configured through command-line arguments. Use the -h or --help option to see the available command-line options:

```
python ./tools/autobild/autobuild.py --help
```

```
usage: autobuild.py [-h] [-w MAX_WORKS] [-c CLEAN_EN] [-p PRJ_PATH] [-s SAVE_PATH] [-d DEL_BUILD] [-v VERIFIED_BUILD] [-f FORMAT_CHECK] [-m MAP_ANALYZER] [-i IGNORE_BUILD] [-r RELEASE_RM]

Autobuild script for building projects.

optional arguments:
  -h, --help            show this help message and exit
  -w MAX_WORKS, --max_works MAX_WORKS
                        Set the maximum number of workers to use for concurrent processing. Default is 1.
  -c CLEAN_EN, --clean_en CLEAN_EN
                        Specify whether to enable cleaning of the build environment before the build process. Accepted values are "true" or "false". Default is "true".
  -p PRJ_PATH, --prj_path PRJ_PATH
                        Specify the path to the directory containing example projects. Default is the "./examples" directory within the current working directory.
  -s SAVE_PATH, --save_path SAVE_PATH
                        Specify the directory where the generated binary files will be saved. Default is null.
  -d DEL_BUILD, --del_build DEL_BUILD
                        Specify whether to delete the build directory after compilation Accepted values are "true" or "false". Default is "false".
  -v VERIFIED_BUILD, --verified_build VERIFIED_BUILD
                        Specify whether to quickly compile the most recently submitted affected project Accepted values are "true" or "false". Default is "false".
  -f FORMAT_CHECK, --format_check FORMAT_CHECK
                        Specify whether recently submitted files have been formatted Accepted values are "true" or "false". Default is "false".
  -m MAP_ANALYZER, --map_analyzer MAP_ANALYZER
                        Specify the type of map analyzer to use. Set "true" to enable the map analyzer, or "false" to disable it. Default is "false".
  -i IGNORE_BUILD, --ignore_build IGNORE_BUILD
                        Specify whether to enable ignore compilation. Set "ture" to enable or "false" to disable it. Default is "false".
  -r RELEASE_RM, --release_rm RELEASE_RM
                        Specify whether release should execute file deletion. Set "ture" to enable or "false" to disable it. Default is "false".
```