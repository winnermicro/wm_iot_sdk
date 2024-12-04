
.. _Bootloader:

Bootloader
==============

Introduction
--------------

Overview
~~~~~~~~~~

.. only:: w800
    
    This document provides a brief description of the Bootloader function and its usage instructions, intended for developers and designers to understand the functionalities of the bootloader.


Terminology Definitions
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - Terminology
     - Definitions

   * - CRC
     - Cyclic Redundancy Check

   * - IMAGE
     - Binary File

   * - MAC
     - Medium Access Control

   * - QFLASH
     - Quad-SPI Flash

   * - RAM
     - Read-Write Memory

   * - ROM 
     - Read-Only Memory

   * - UART
     - Universal Asynchronous Receiver-Transmitter 

   * - MB
     - Mega Byte

   * - KB
     - Kilo Byte

Bootloader Flowchart
-----------------------

.. figure:: ../../_static/component-guides/bootloader/bootloader_en.svg
    :align: center
    :alt: Bootloader Flowchart

Bootloader Functionality
--------------------------

Description of Bootloader Functionality
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

IMAGE Boot Part
^^^^^^^^^^^^^^^^^

QFlash Working Mode Switch
"""""""""""""""""""""""""""

For unknown Flash manufacturers, when switching from ROM to QFlash, it is necessary to switch the QFlash working mode of QFlash: from 1-wire mode to 4-wire mode.

IMAGE Integrity Verification
""""""""""""""""""""""""""""

(1) Perform CRC verification on the IMAGE header and the complete content of the IMAGE.
(2) If the IMAGE compiled by wm.py is signed, RSA signature verification will be performed on the complete content of the IMAGE.

Code Decompression
"""""""""""""""""""

In cases where customers compress code to save space, decompression is required based on the compression algorithm used. 
Consideration should be given to which compression and decompression algorithms to use, as well as their compression rates. Only the XZ algorithm is supported.

Code Relocation
""""""""""""""""

The decompressed code needs to be placed in the specified running position, determined based on the header information of the IMAGE.

Code Decryption Setup
""""""""""""""""""""""

For encrypted code, decryption information needs to be configured.

Code Jump Running
""""""""""""""""""

After code relocation, determine the reset vector address of the code and jump from the bootloader to the user's image for execution.