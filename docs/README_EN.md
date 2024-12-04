# WinnerMicro Documentation 

* [中文](README.md)

This is [Documentation Center](https://doc.winnermicro.net)  for the WM IoT SDK.

## Installing dependencies

- Install doxygen, depending on the system being used
- Install python at least version 3.8
- install python library dependencies in docs directory: `python -m pip install --user -r requirements.txt`

## Compiling Chinese documentation

Under `wm_iot_sdk/docs`,  execute the command `python build.py` to compile the documentation.

Build results are placed in `wm_iot_sdk/docs/build` directory, 

sorted by chip type, language type, and version type in their respective `html` folder.

> Note: Clearing the build results requires manually deleting the `build` folder.
>

## View documentation

Place the html file in the `build` folder on the web server, and then use a browser to access the web server.

> Note: Can directly local double-click the web page to open reading use.
>

## Notes

This document is built using Sphinx technology. The Windows command line compilation environment needs to be updated to version 0.0.13 to support the current document compilation, otherwise you need to install Sphinx related tools.
