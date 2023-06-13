# Open Source RIS
An open source linearly polarized Reconfigurable Intelligent Surface (RIS) with 1-bit phase control for the frequency range of 5 GHz WiFi (5.15 - 5.875 GHz)

<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/879b14ec-e073-4064-8092-d0f0a39c2f05" alt="Simplified unit cell" width="60%" />
  <br />
  Fig. 1: Open source RIS with 16×16=256 elements, front side
</p>

<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/879b14ec-e073-4064-8092-d0f0a39c2f05" alt="Simplified unit cell" width="60%" />
  <br />
  Fig. 2: Back side of the RIS
</p>

## Unit Cell Design
The RIS unit cell consists of a linearly polarized pin-fed patch antenna, realized on a printed circuit board (PCB). Low-cost FR4 substrate with a specified dielectric constant of ϵr = 4.6 and dissipation factor tan δ = 0.028 is used. Fig. 3 shows the simplified unit cell with its dimensions. The circuit network generates a binary switchable reflection coefficient by using a BGS12P2L6 single pole, double throw (SPDT) RF switch from Infineon Technologies.

<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/89da5c8c-9dff-4ede-b9c3-447b4d54111c" alt="Simplified unit cell" width="40%" />
  <br />
  Fig. 3: Simplified unit cell
</p>

## Measured Surface Reflection Coefficient
The surface reflection coefficient of the RIS is measured with a vector network analyzer. During the measurement, the RIS is located in the aperture of a large horn antenna, which is specially designed for surface reflection coefficient measurements. Thus, the propagation vectors of the incident and reflected waves are perpendicular to the surface (normal incidence and reflection). Measuremetns are done for the two states where all elements are turned OFF or ON. A metal plate in size of the RIS is used for normalization of the measuement.
<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/ed71ec81-0a76-41ab-a6e4-504c02d85109" alt="Simplified unit cell" width="40%" />
  <br />
  Fig. 4: Setup for measurement of surface reflection coefficient
</p>

Fig. 5 shows the magnitude of the measured reflection coefficient. In the WiFi frequency bands from 5.15 - 5.875 GHz, the worst case surface reflection coefficient is −5.2 dB at 5.56 GHz for the OFF state and −4.8 dB at 5.15 GHz for the ON state.
<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/f869c1fb-a102-4f43-b495-82bf06ad6e1b" alt="Simplified unit cell" width="40%" />
  <br />
  Fig. 5: Magnitude response
</p>

Fig. 6 shows the measured phase response as well as the phase difference between the all ON and all OFF states. The phase difference is wrapped to values between 0° and 180° and reaches its maximum at 5.53 GHz. It is worse at 5.875 GHz with a value of 92°.
<p align="center">
  <img src="https://github.com/mheinri/OpenSourceRIS/assets/122888316/01f5b045-c227-4d03-847d-c502eb1adf48" alt="Simplified unit cell" width="40%" />
  <br />
  Fig. 6: Phase response and phase difference
</p>

## Control Interfaces
A control board is attached to the back side of the RIS. With this, the RIS can be connected to a PC and controlled via a USB connection. Alternatively, it can be operated wirelessly with a battery and controlled via Bluetooth Low Energy.

## To be added
The following information will be added soon:
* Information on control syntax
* Schematic of the RF PCB
* Manufacturing information

## Publications
* M. Heinrichs, and R. Kronberger, "Open Source Reconfigurable Intelligent Surface for the Frequency Range of 5 GHz WiFi", handed in for ISAP 2023
* M. Heinrichs, F. Pirlet, and R. Kronberger, "Measurement System and Methodology for RIS Evaluation", handed in for ISAP 2023

## Akcnowledgment
This work was funded by the German Federal Ministry of Education and Research (BMBF) in the framework of Project MetaSec (Förderkennzeichen 16KIS1236).
