# Manufacturing Information
Open Source RIS is designed for the JLC04161H-7628 impedance-controlled PCB stackup from JLCPCB ([jlcpcb.com](https://jlcpcb.com/)). It is highly recommended to use the same manufacturer and PCB stackup for the RF PCB in order to get comparable results! For the controller PCB any suitable 4-layer PCB stackup may be used.

Recommended PCB stackup is as follows:

|Copper-Layer|Material|Thickness|Layer usage (RF PCB)|Layer Usage (Controller PCB)|
|-|-|-|-|-|
|1|Copper + gold plating|0.035 mm|Signals and components|Signals and components|
| |FR4 (7628 material)|0.210 mm| | |
|2|Copper|0.015 mm|Groundplane|Groundplane|
| |FR4 (core material)|1.065 mm| | |
|3|Copper|0.015 mm|Empty (copper removed)|+3.3 V|
| |FR4 (7628 material)|0.210 mm| | |
|4|Copper + gold plating|0.035 mm|Patches|Signals|

Dielectric constant of prepreg material and core material must be equal:
* Dielectric constant (according to manufacturer): 4.6
* Dielectric constant (measured and used for simulation): 4.8

# Known Issue
It has been observed that in some cases the board-to-board connections do not work. Due to its large size, the RF PCB is not absolutely planar. By mounting the RIS, e.g. on an aluminum frame, the RF PCB bends. This affects the pin header connection, causing it to fail in some cases.

We have therefore soldered the controller PCB directly to the RF PCB by omitting the female pin heads. The distance between RF PCB and controller PCB should remain at least 3 mm to avoid undesirable effects on the unit cells below.
