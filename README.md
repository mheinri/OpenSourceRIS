# Open Source RIS
An open source linearly polarized Reconfigurable Intelligent Surface (RIS) with 1-bit phase control for the frequency range of 5 GHz WiFi (5.15 - 5.875 GHz).

<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/56814852-78b3-49b1-a10c-a79a4851f878" alt="Open source RIS" width="60%" />
  <br />
  Fig. 1: Open source RIS with 16×16=256 elements, front side
</p>

<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/84f6323b-6554-4300-bc10-737ae207595c" alt="RIS backside" width="60%" />
  <br />
  Fig. 2: Back side of the RIS with controller board attached
</p>

## Unit Cell Design
The RIS unit cell consists of a linearly polarized pin-fed patch antenna, realized on a printed circuit board (PCB). Low-cost FR4 substrate with a specified dielectric constant of ϵr = 4.6 and dissipation factor tan δ = 0.028 is used. Fig. 3 shows the simplified unit cell with its dimensions. The circuit network generates a binary switchable reflection coefficient by using a BGS12P2L6 single pole, double throw (SPDT) RF switch from Infineon Technologies.

<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/717799f0-ef0e-4d5b-8a60-6f85ebf67e0f" alt="Simplified unit cell" width="40%" />
  <br />
  Fig. 3: Simplified unit cell
</p>

## Surface Reflection Coefficient
The surface reflection coefficient of the RIS is measured with a vector network analyzer. During the measurement, the RIS is located in the aperture of a large horn antenna, which is specially designed for surface reflection coefficient measurements. Thus, the propagation vectors of the incident and reflected waves are perpendicular to the surface (normal incidence and reflection). Measuremetns are done for the two states where all elements are turned OFF or ON. A metal plate in size of the RIS is used for normalization of the measuements.
<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/73685b09-632b-4027-956d-4386dfb752a8" alt="Measurement setup" width="40%" />
  <br />
  Fig. 4: Setup for measurement of surface reflection coefficient
</p>

Fig. 5 shows the magnitude of the measured reflection coefficient. In the WiFi frequency bands from 5.15 - 5.875 GHz, the worst case surface reflection coefficient is −5.2 dB at 5.56 GHz for the OFF state and −4.8 dB at 5.15 GHz for the ON state.
<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/b83f33f8-22d1-485b-8765-e585e8c5e22f" alt="Magnitude response" width="60%" />
  <br />
  Fig. 5: Magnitude response
</p>

Fig. 6 shows the measured phase response as well as the phase difference between the all ON and all OFF states. The phase difference is wrapped to values between 0° and 180° and reaches its maximum at 5.53 GHz. It is worse at 5.875 GHz with a value of 92°.
<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/caa335ad-a6ea-4615-8e6b-71c7512155d5" alt="Simplified unit cell" width="60%" />
  <br />
  Fig. 6: Phase response and phase difference
</p>

## Power Supply
Power is supplied either via USB or via an external voltage source, e.g. a power supply unit, a battery or a rechargeable accumulator. The external voltage can be supplied via a coaxial power connector (2.1 x 5.5 mm), where the positive potential is applied to the inner pin of the connector. The recommended voltage range is between 9 V and 24 V DC. Power consumption with all elements ON as approx. 2 W.

The external voltage can be measured and read out, see documentation of the corresponding command below. This can be used, for example, to monitor the charge level of a battery.

## Control Interfaces
A controller board is attached to the back side of the RIS. With this, the RIS can be connected to a PC and controlled via a USB connection. Alternatively, it can be operated wirelessly with a battery and controlled via Bluetooth Low Energy (BLE). Fig. 7 shows the block diagram of the control unit.

<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/b56138ef-4683-4436-9eda-e91e34c1d54c" alt="Block diagram" width="60%" />
  <br />
  Fig. 7: Block diagram
</p>

### USB interface
When using the USB interface, a virtual COM port (VCP) driver for the FT231XS USB-to-UART converter has to be installed. If the driver is not already installed or does not install automatically, it can be downloaded from the homepage of manufacturer FTDI (currently available at [https://ftdichip.com/drivers/vcp-drivers/](https://ftdichip.com/drivers/vcp-drivers/)). The VCP driver is provided for Windows, Linux and Mac OS.

Settings for the virtual COM port are as follows:
* 8 data bits
* 0 parity bits
* 1 stop bit
* 115.2 kBd/s

### BLE interface
A Proteus-III Bluetooth module from Würth Elektronik is used for control via Bluetooth. This is configured as a Bluetooth peripheral and must be coupled with a Bluetooth central, for example a PC with a Bluetooth interface installed. If the RIS is supplied with power, a new Bluetooth device appears in the search list of the PC. The device identifier or Bluetooth name of the RIS has the format A-xxxxxx, where xxxxxx stands for three bytes of the MAC address of the Bluetooth module in hexadecimal format (e.g. A-3163D1). These three bytes can be taken from the label on the Bluetooth module as "ID". The green LED next to the Bluetooth module flashes as long as the module has not been paired and is in advertising mode.

If pairing is started, a Static Pass Key must be entered. That Key hast to be set in advance via the USB interface and the `!BT-Key` command described below. If the pairing was successful, the green LED next to the Bluetooth module lights up permanently.

After pairing, the write and read services of the Bluetooth protocol must be activated in order to exchange data with the Bluetooth module. Once the services have been activated, the red LED next to the Bluetooth module lights up continuously. How exactly the services are activated depends on the software used to control the Bluetooth interface on the PC. Example code for MATLAB is provided under [Software/Control Software/MATLAB Code Examples](/Software/Control%20Software/MATLAB%20Code%20Examples/).

### Command syntax
All commands are ASCII encoded and are therefore easy to read. Each command starts with an exclamation mark (`!`) or a question mark (`?`) as start character followed by the actual command content. The command is then terminated at the end with a newline character (`␤`). A response from the RIS starts with a number sign (`#`) and is terminated with a newline cahracter as well. It is not necessary to be case sensitive with commands.

The following commands are available:
* Setting a pattern: `!0x...`
* Reading the currently configured pattern: `?Pattern`
* Reading the external supply voltage: `?Vext`
* Reading the serial number: `?SerialNo`
* Resetting the RIS: `!Reset`
* Setting the Static Pass Key for Bluetooth: `!BT-Key`

#### Setting a pattern
Each RIS element has been assigned a number between 1 and 256. Looking at the front of the surface as in Fig. 1, the first element is located in the top left corner. The other elements are arranged in reading direction with ascending number.
Examples: The second element is to the right of the first element, the 16th element is in the top right corner, the 17th element is on the far left in the second row from the top, and the 256th element is the one in the bottom right corner.
Essentially, a 256-bit number is sent to the RIS as a control command. Here, each bit represents the state of a specific RIS element. With a 0, the corresponding element is deactivated and the LED on the back is not lit and with a 1, the element is activated and the LED lights up. The assignment of the bit indices to the unit cells can be taken from the table below.

|Bit-Index|255 (MSB)|254|253|...|2|1|0 (LSB)|
|---|---|---|---|---|---|---|---|
|**Element no.**|1|2|3|...|254|255|256|

The control command for setting a pattern is formed on the basis of this 256-bit number. The command starts with an exclamation mark (`!`) as start character, followed by the 256-bit number in hexadecimal format. The hex prefix (`0x`) is specified at the beginning for better readability. The command is then terminated with a newline character (`␤`) at the end.

|Example Command|Set Pattern|
|---|---|
|`!0x0000000000000000000000000000000000000000000000000000000000000000␤`|All elements inactive|
|`!0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF␤`|All elements active|
|`!0x8000000000000000000000000000000000000000000000000000000000000000␤`|Element 1 active|
|`!0x0000000000000000000000000000000000000000000000000000000000000001␤`|Element 256 active|
|`!0xFF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00␤`|Left half of the RIS active|
|`!0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000000000000000000000000␤`|Upper half of the RIS active|

If the pattern was set successfully, the RIS sends `#OK␤` as confirmation. If no response is sent by the RIS, either an invalid command was used or an error occurred. In this case, the configured pattern of the RIS remains unchanged.

#### Reading the currently configured pattern
To read out the currently set pattern, the command `?Pattern␤` is used. The RIS then responds with `#0X012...DEF␤`, where `012...DEF` is a hexadecimal 256-bit long number. The structure of this number is the same as for setting a pattern.

|Example Command|Example Response from RIS|
|---|---|
|`?Pattern␤`|`#0X00007FFE40025FFA500A57EA542A55AA55AA542A57EA500A5FFA40027FFE0000␤`|

#### Reading the external supply voltage
In order to read out the external supply voltage that is present at the coaxial power connector, the command `?Vext␤` is used. The RIS then responds with `#00.00 V␤`, where `00.00` is a fixed-point number for the measured voltage in Volts.

|Example Command|Example Response from RIS|
|---|---|
|`?Vext␤`|`#09.14 V␤`|

#### Reading the serial number
Reading the serial number of a RIS board is done by sending the `?SerialNo␤` command. The RIS then responds with `#SerNo: 123␤`, where `123` is a fixed length serial number of up to three digits. For serial numbers with less digits, leding spaces are transmitted as well.

|Example Command|Example Response from RIS|
|---|---|
|`?SerialNo␤`|`#SerNo:  12␤`|

#### Resetting the RIS
The RIS can be reset by sending the `!Reset` command. The microcontroller then performs a software reset. The RIS is back up as soon as a `#READY␤` is received at the end of the boot information.

An example full response of the RIS after resetting or power cycling is as follows:
```
␤
Open Source RIS␤
Firmware version: 1.1␤
Serial no.:  12␤
Row count: 16␤
Column count: 16␤
␤
#READY!␤
```

Note that an established Bluetooth connection gets lost during a reset, so that a reinitialization of the BLE connection is required.

#### Setting the Static Pass Key for Bluetooth
The Static Pass Key can only be set via the USB interface. This command cannot be executed via Bluetooth. The Static Pass Key is non-volatile and remains unchanged after power cycling the RIS or resetting it via the `!Reset` command.
To set the Static Pass Key, the command `!BT-Key=123456␤` is used, where `123456` is exemplary for the new Key. The Static Pass Key has a fixed length of six decimal digits, all of which must be specified. Valid is therefore a number between zero and 999999.

|Example Command|New Static Pass Key|
|---|---|
|`!BT-Key=000000␤`|000000|
|`!BT-Key=123456␤`|123456|
|`!BT-Key=054860␤`|054860|

If the Static Pass Key was set successfully, the RIS sends `#OK␤` as confirmation. If the RIS sends `#ERROR␤`, either an invalid key was specified or an error occurred. Time between sending the command and receiving the response may take several seconds.

## Publications
* [M. Heinrichs, A. Sezgin, and R. Kronberger, "Open Source Reconfigurable Intelligent Surface for the Frequency Range of 5 GHz WiFi", handed in for ISAP 2023](https://github.com/mheinri/OpenSourceRIS/blob/main/Open%20Source%20Reconfigurable%20Intelligent%20Surface%20for%20the%20Frequency%20Range%20of%205%20GHz%20WiFi.pdf)
* M. Heinrichs, F. Pirlet, and R. Kronberger, "Measurement System and Methodology for RIS Evaluation", handed in for ISAP 2023

## Acknowledgment
Many thanks to the Institute of Electronic Circuits at Ruhr University Bochum for the extensive help with assembling the printed circuit boards.
This work was funded by the German Federal Ministry of Education and Research (BMBF) in the framework of Project MetaSec (Förderkennzeichen 16KIS1236).
