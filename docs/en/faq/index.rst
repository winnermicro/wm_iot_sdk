FAQ
===================

This chapter summarizes the common issues encountered by users during usage!


Resources
-----------------

* You can ask your questions in the `WinnerMicro Q&A Community <http://ask.winnermicro.com/>`_ and access community resources.

* Interact through QQ groups with the following group numbers: 570560884、335112873、923841628。

* For developing applications on the Arduino platform, please refer to `w80x_arduino <https://github.com/board707/w80x_arduino>`_。

* For developing applications on the MicroPython platform, please refer to  `micropython-w806 <https://gitee.com/haorongMango/micropython-w806>`_。


Compilation
-------------------

* Can I compile without using your cmd command line?

  Absolutely, the cmd command line is just an example of a compilation environment we provide. Users can DIY their preferred compilation environment based on this. Because tastes differ, there's no such thing as a definitely best environment. 
  Only the one that suits you is a good compilation environment. Users can refer to the content in the "Setting Up Compilation Environment" section to build an environment that suits them, but for beginners, it is still recommended to download and use the integrated package.


* Why is installing Python dependencies slow?

  You can speed up the by replacing the default source.For example, Chinese users can choose Tsinghua's source:

  ::

    python -m pip install --user -r wm_iot_sdk/tools/wm/requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple
  
  Please choose and use other sources as needed.

* What should I do if my code changes don't take effect?

  The two most common problems are that after switching to a different version of WM IoT SDK, or after copying the sample project to another place, the changes in the code never take effect.

  You need to check the current version of WM IoT SDK and the correct path of the project, you can do it by the following steps:
  
  1. Delete the ``build`` folder in the project directory.

  2. Execute a build and observe the information generated by the build.

  3. Check the values of ``SDK_PATH`` and ``PROJECT_PATH`` (corresponding to the current WM IoT SDK path and project path, respectively) in the compile message to make sure that they are the same as what you expect.

  4. If it is not the same, please refer to the section on building compilation environment to reconfigure the environment variables related to WM IoT SDK.

     If you are using VS Code, after restarting VS Code, please reconfigure the path of WM IoT SDK in the plug-in settings, and then go to the project to start compiling.