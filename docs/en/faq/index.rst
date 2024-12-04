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

  pip install package -i https://pypi.tuna.tsinghua.edu.cn/simple
  
Please choose and use other sources as needed.
