
# Simple PYNQ KV260 tutorial: Porting SYDR C-based design into FPGA via Xilinx HLS
 

## Description

This repository contains a simple PYNQ design that calculates correlation coefficients from two given signals. This function will be implemented in FPGA via Xilinx HLS, it is called ''getCorrelator''.

The tutorial will cover the PYNQ design flow, including how to port a C function into HLS styled C in Vitis HLS, how to Vivado block design for KV260, how to create a PYNQ overlay, how to use the overlay in python environment.




### The contributors of this tutorial are:
- `Jie Lei`, Universitat Politècnica de Valencia, Spain
- `Hans Jakob Damsgaard`, Tampere University, Finland 
- `Antoine Grenier`, Tampere University, Finland 


 
 ---
[Tutorial 1: Porting a C function to Xilinx HLS, with stream interface](/Readme_getCorrelator.md)

[Tutorial 2: Using Xilinx FFT IP with DMA and AXI mater interface](/Readme_X_FFT.md)

[Tutorial 2: DMA with HLS stream based IP](/dma_ip_stream.md)

---


## Acknowledgement


<img align="right" src="https://raw.githubusercontent.com/aproposorg/KV260-PYNQ-tutorial/main/image/HWSYDR.png" height="140">

- This work is also within the scope of the Hardware acceleration for GNSS receiver project called Hard Sydr. The Python version of this project is called Sydr. https://github.com/aproposorg/sydr

- The work is conducted within the project APROPOS. This project has received funding from the European Union’s Horizon 2020 (H2020) Marie Sklodowska-Curie Innovative Training Networks H2020-MSCA-ITN-2020 call, under the Grant Agreement no 956090. Project link: https://apropos-project.eu/

- License: MIT Copyright (c) 2023 APROPOS


### Source code may contain work from:

- ``Sydr`` An open-source GNSS Software Defined Radio for algorithm benchmarking. https://github.com/aproposorg/sydr developed by Antoine Grenier.
- AMD Xilinx tutorial.


