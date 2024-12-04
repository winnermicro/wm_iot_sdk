.. _ble_mesh_switch_and_light_sample_en:

mesh_switch_and_light
######################

Overview
********

This sample demonstrates Bluetooth mesh functionality. 

- The example includes several standard Mesh models and a generic on/off model.

- It supports provisioning operations through both broadcast mode and GATT mode.

- It supports entering authorization information through the console.

- It supports sending on/off commands through the console.

- It supports publish and subscribe functionalities.

- This example also demonstrates more visually the working state of the Mesh node by outputting the high and low states of different GPIOs.
   
.. csv-table:: Correspondence between node operating state and GPIO output state
    :header: "Node operating status", "WM_GPIO_NUM_16(PB0)", "WM_GPIO_NUM_17(PB1)", "WM_GPIO_NUM_18(PB2)"
    :widths: 10, 30, 30, 30

    "Waiting Provision ", "Low", "Low", "High-low switching, 500ms interval"
    "Identity ", "Low", "High-low switching, 200ms interval", "Low"
    "Turn On ", "High", "High", "High"
	"Turn Off ", "Low", "Low", "Low"
	"Idle ", "Low", "Low", "High-low switching, 1000ms interval"


- If you are using `W800-Arduino Development Board <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/w800_arduino.html>`_, the mesh node's operating state and the color of the tri-color LEDs correspond to the following table (make sure the tri-color LED toggle switch is ON)

.. csv-table:: Correspondence between node operating status and LED colors
    :header: "Node operating status", "Tri-color LED light"
    :widths: 10, 30

    "Waiting Provision ", "Blinking blue, 500ms interval"
    "Identity ", "Blinking green, 200ms interval"
    "Turn On ", "ON"
    "Turn Off ", "OFF"
    "Idle ", "Blinking blue, 1000ms interval"


User interface (cli commands)
******************************

mesh_prov_input_num:
   * Enter the authorization code in decimal format. e.g. ``mesh_prov_input_num 6789`` 

mesh_prov_input_str:
   * Enter the authorization code in string format. e.g. ``mesh_prov_input_num asui``

mesh_ctrl:
   * Control the node to turn on and off the light command. Command Format: ``mesh_ctrl dest val ack`` , dest is element address of mesh node (decimal format)，val is 1/0 (on off state)，ack is 1/0 (commadn with/without ack)。 

Requirements
************

* A board with Bluetooth LE Mesh support
* nRF Mesh APP


Building and Running
********************

Example Location：``examples/bluetooth/mesh_switch_and_light``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_

Running Result
***************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (3) main: ver: 2.1.1dev build at Oct 29 2024 17:22:01
	[I] (4) bt: Mesh node demo running
	[I] (27) bt: No ID address. App must call settings_load()
	[I] (27) bt: Bluetooth initialized
	[I] (156) bt: Identity: 28:6D:CD:55:E5:90 (public)
	[I] (157) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (157) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (187) bt: Mesh initialized
	
2. Run nRF Mesh APP, and add a node with the ** device address (28:6D:CD:55:E5:90)** 

.. figure:: assert/mesh_add_node.svg
    :align: center
	
3. As shown in Figure 5, when selecting the OOB Type, you can choose either No OOB or an OOB type supported by Mesh devices. If No OOB is selected, skip this step. If an OOB type supported by Mesh devices is chosen, after confirming on the nRF Mesh APP, enter the OOB number printed in the development board's log and confirm.

  Output the OOB Number on board  ::

	[I] (283203) bt: OOB Number: 9001


.. figure:: assert/mesh_auth.svg
    :align: center	

4. After the Mesh node is successfully added, the nRF Mesh APP can view the element information of the node.

.. figure:: assert/mesh_elements.svg
    :align: center

5. At the same time, the development board outputs the primary element address (default address of the node) of the Mesh node.

.. code-block:: console

	[I] (301084) bt: Primary Element: 0x0010
	[I] (301091) bt: prov_complete: addr 0x0010

6. On the nRF Mesh APP, bind an APP Key to the Generic On Off Server model. 

.. figure:: assert/mesh_onoff_server.svg
    :align: center

7. After successfully binding the APP Key to the model, you can execute on/off control on the APP (as shown in Figure 14), and the development board will output the on/off state.	

.. code-block:: console

	[I] (365844) bt: onoff set state 1
	[I] (367294) bt: onoff set state 0

8. On the nRF Mesh APP, bind an APP Key to the Generic On Off Client model

.. figure:: assert/mesh_onoff_client.svg
    :align: center


9. After successfully binding the APP Key to the model,use ``mesh_ctrl`` command to control a node onoff state within the mesh network. e.g. turn on a Generic On Off Server with address of 12 (element address) and ack is needed 
::

	mesh_ctrl 12 1 1
	[I] (55977973) bt: onoff ctrl dest 6 val 1
	[I] (55978132) bt: onoff state 1 [56114,27] max_delay 237 ms
