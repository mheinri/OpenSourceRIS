# Manufacturing Information
Open Source RIS is designed for the JLC04161H-7628 impedance-controlled PCB stackup from JLCPCB ([jlcpcb.com](https://jlcpcb.com/)). It is highly recommended to use the same manufacturer and PCB stackup for the RF PCB in order to get comparable results! For the controller PCB any suitable 4-layer PCB stackup may be used.

Recommended PCB stackup is as follows:

|Copper-Layer|Material|Thickness|Layer Usage (RF PCB)|Layer Usage (Controller PCB)|
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

# Settings for Ordering at JLCPCB
The following order options have been used when ordering from JLCPCB. Irrelevant options are omitted.
## RF PCB
* Base Material: FR4
* Layers: 4
* Dimension: 360 mm * 247 mm
* Product Type: Industrial/Consumer electronics
* Different Design: 1
* Delivery Format: Single PCB
* PCB Thickness: 1.6 mm
* Impedance Control: yes JLC04161H-7628
* Material Type: TG155
* Via Covering: Tented
* Surface Finish: ENIG Gold Thickness: 1 U'' (ENIG is preferred over HASL)
* Deburring/Edge routing: No
* Outer Copper Weight: 1 oz
* Inner Copper Weight: 0.5 oz
* Gold Fingers: No
* Flying Probe Test: Fully Test
* Castellated Holes: no
* Remove Order Number: Yes (but is not required)
* Min via hole size/diameter: 0.2 mm/(0.3/0.35 mm)
* 4-Wire Kelvin Test: Yes
* Paper between PCBs: Yes (but is not required)
* Appearance Quality: IPC Class 2 Standard
* Silkscreen Technology: Ink-Jet/Screen Printing Silkscreen
* Board Outline Tolerance: +/- 0.2 mm (Regular)

## Controller PCB
Same as above with Dimension 105 mm * 95 mm. Impedance controlled stackup is not required as the controller PCB is not relevant for the RF properties.

# Known Issue
It has been observed that in some cases the board-to-board connections do not work. Due to its large size, the RF PCB is not absolutely planar. By mounting the RIS, e.g. on an aluminum frame, the RF PCB bends. This affects the pin header connection, causing it to fail in some cases.

We have therefore soldered the controller PCB directly to the RF PCB by omitting the female pin heads. The distance between RF PCB and controller PCB should remain at least 3 mm to avoid undesirable effects on the RIS elements below.
