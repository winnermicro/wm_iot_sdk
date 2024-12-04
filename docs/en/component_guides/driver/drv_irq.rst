
.. _irq:

IRQ
=============

Introduction
--------------

  IRQ (Interrupt Request) refers to the interrupt requests from devices. The driver provides a set of functions for managing hardware and software interrupts. This module provides interfaces for registering and deregistering both hardware and software interrupts, enabling and disabling interrupts, and managing the pending status of interrupts.

Function List
--------------

    1. :ref:`Register Hardware Interrupt Vector <irq_attach_hw_vector>` — Attach a hardware interrupt vector and replace the software vector.
    2. :ref:`Deregister Hardware Interrupt Vector <irq_detach_hw_vector>` — Detach a hardware interrupt vector.
    3. :ref:`Register Software Interrupt Vector <irq_attach_sw_vector>` — Attach a software interrupt vector and callback function.
    4. :ref:`Deregister Software Interrupt Vector <irq_detach_sw_vector>` — Detach a software interrupt vector.
    5. :ref:`Disable and Save Interrupt State <irq_save>` — Disable global interrupts and save the previous state.
    6. :ref:`Restore Interrupt State <irq_restore>` — Enable global interrupts using the previously saved state.
    7. :ref:`Enable Interrupt <irq_enable>` — Enable a specified interrupt.
    8. :ref:`Disable Interrupt <irq_disable>` — Disable a specified interrupt.
    9. :ref:`Set Interrupt Pending <irq_set_pending>` — Set a specified interrupt to the pending state.
    10. :ref:`Clear Interrupt Pending <irq_clear_pending>` — Clear the pending state of a specified interrupt.
    11. :ref:`Set Wakeup Interrupt <irq_set_wakeup>` — Set a specified interrupt as a wakeup interrupt.

Function Overview
-------------------------

    The diagram below illustrates the management of the interrupt vector table.

    .. figure:: ../../../_static/component-guides/driver/irq_en.png
        :align: center
        :alt: Interrupt Vector Table Diagram

    
	In interrupt management, a set of software interrupt registration mechanisms is provided. In principle, users only need to register the software table. For performance considerations, interfaces for directly writing to the hardware vector table are also provided. Note that when registering both hardware and software interrupts on the same interrupt vector, the hardware interrupt has higher priority and will overwrite the corresponding software interrupt, causing the software interrupt to fail. If a hardware interrupt is registered first, followed by a software interrupt, the software interrupt cannot be registered.

Main Functions
-----------------

.. _irq_attach_hw_vector:

Registering Hardware Interrupt Vector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call the function ``wm_drv_irq_attach_hw_vector()`` to register a hardware interrupt vector and replace the software vector. Example:

    .. code:: c

        void irq_callback(void) {
        }

        wm_drv_irq_attach_hw_vector(WM_IRQ_I2C, irq_callback);

    The first parameter is the interrupt number. The W800 chip has 32 interrupt numbers, each corresponding to an interrupt source on the W800 chip. The type is ``wm_irq_no_t``.

    The second parameter is the interrupt callback function, of type ``wm_drv_hw_irq_handle``.

.. _irq_detach_hw_vector:

Detaching a Hardware Interrupt Vector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

     The function ``wm_drv_irq_detach_hw_vector()`` is called to detach a hardware interrupt vector. An example is as follows:

    .. code:: c

        wm_drv_irq_detach_hw_vector(WM_IRQ_I2C);

    The parameter is the interrupt number, of type ``wm_irq_no_t``.

.. _irq_attach_sw_vector:

Registering a Software Interrupt Vector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The function ``wm_drv_irq_attach_sw_vector()`` is called to attach a software interrupt vector and a callback function. An example is as follows:

    .. code:: c

        void irq_callback(void *arg) {
        }

        wm_drv_irq_attach_sw_vector(WM_IRQ_I2C, irq_callback, NULL);

    The first parameter is the interrupt number, of type ``wm_irq_no_t``.

    The second parameter is the interrupt callback function, of type ``wm_drv_sw_irq_handle``.

    The third parameter is user data, of type ``void *``.

.. _irq_detach_sw_vector:

Detaching a Software Interrupt Vector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The function ``wm_drv_irq_detach_sw_vector()`` is called to detach a software interrupt vector. An example is as follows:

    .. code:: c

        wm_drv_irq_detach_sw_vector(WM_IRQ_I2C);

    The parameter is the interrupt number, of type ``wm_irq_no_t``.

.. _irq_save:

Saving and Disabling Interrupt State
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The function ``wm_drv_irq_save()`` is called to disable interrupts and save the previous state. An example is as follows:

    .. code:: c

        uint32_t irq_state;
        irq_state = wm_drv_irq_save();

    This function returns the previous interrupt state, of type ``uint32_t``.

.. _irq_restore:

Restoring Interrupt State
^^^^^^^^^^^^^^^^^^^^^^^^^

    The function ``wm_drv_irq_restore()`` is called to enable interrupts using the previously saved state. An example is as follows::

    .. code:: c

        uint32_t irq_state;
        irq_state = wm_drv_irq_save();

        wm_drv_irq_restore(irq_state);

    Here, ``irq_state`` is used to save the interrupt state, which can then be used to restore the interrupt state. The parameter is the previously saved interrupt state, of type  ``uint32_t``.

.. _irq_enable:

Enabling an Interrupt
^^^^^^^^^^^^^^^^^^^^^^

    The function ``wm_drv_irq_enable()`` is called to enable a specified interrupt.  An example is as follows:

    .. code:: c

        wm_drv_irq_enable(WM_IRQ_I2C);

    The parameter is the interrupt number, of type ``wm_irq_no_t``.

.. _irq_disable:

Disabling an Interrupt
^^^^^^^^^^^^^^^^^^^^^^^^

    The function ``wm_drv_irq_disable()`` is called to disable a specified interrupt. An example is as follows:

    .. code:: c

        wm_drv_irq_disable(WM_IRQ_I2C);

    The parameter is the interrupt number, of type ``wm_irq_no_t``.

.. _irq_set_pending:

Setting an Interrupt Pending
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The function``wm_drv_irq_set_pending()`` is called to set a specified interrupt to the pending state. An example is as follows:

    .. code:: c

        wm_drv_irq_set_pending(WM_IRQ_I2C);

    The parameter is the interrupt number, of type ``wm_irq_no_t``.

.. _irq_clear_pending:

Clearing an Interrupt Pending
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The function ``wm_drv_irq_clear_pending()``  is called to clear the pending state of a specified interrupt. An example is as follows::

    .. code:: c

        wm_drv_irq_clear_pending(WM_IRQ_I2C);

    The parameter is the interrupt number, of type ``wm_irq_no_t``.

.. _irq_set_wakeup:

Setting Wakeup Interrupt
^^^^^^^^^^^^^^^^^^^^^^^^^^

    The function ``wm_drv_irq_set_wakeup()`` is called to set a specified interrupt as a wakeup interrupt. When the device is in low-power mode, a wakeup interrupt can trigger the device to resume normal operation from the low-power mode. An example is as follows:

    .. code:: c

        wm_drv_irq_set_wakeup(WM_IRQ_I2C);

    The parameter is the interrupt number, of type ``wm_irq_no_t``.

API Reference
--------------

    To find IRQ-related APIs, please refer to:

    :ref:`label_api_irq`