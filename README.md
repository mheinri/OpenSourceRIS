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

## Control Interfaces
A controller board is attached to the back side of the RIS. With this, the RIS can be connected to a PC and controlled via a USB connection. Alternatively, it can be operated wirelessly with a battery and controlled via Bluetooth Low Energy.

## To be added
The following information will be added soon:
* Information on control syntax
* Schematic of the RF PCB
* Additional manufacturing information

## Publications
* [M. Heinrichs, A. Sezgin, and R. Kronberger, "Open Source Reconfigurable Intelligent Surface for the Frequency Range of 5 GHz WiFi", handed in for ISAP 2023](https://github.com/mheinri/OpenSourceRIS/blob/main/Open%20Source%20Reconfigurable%20Intelligent%20Surface%20for%20the%20Frequency%20Range%20of%205%20GHz%20WiFi.pdf)
* M. Heinrichs, F. Pirlet, and R. Kronberger, "Measurement System and Methodology for RIS Evaluation", handed in for ISAP 2023

## Acknowledgment
Many thanks to the Institute of Electronic Circuits at Ruhr University Bochum for the extensive help with assembling the printed circuit boards.
This work was funded by the German Federal Ministry of Education and Research (BMBF) in the framework of Project MetaSec (Förderkennzeichen 16KIS1236).
