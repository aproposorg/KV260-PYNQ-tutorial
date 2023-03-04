
# Simple PYNQ KV260 tutorial: Porting C-based design into FPGA via Xilinx HLS
 

## Acknowledgement

- The work is conducted within the project APROPOS. This project has received funding from the European Union’s Horizon 2020 (H2020) Marie Sklodowska-Curie Innovative Training Networks H2020-MSCA-ITN-2020 call, under the Grant Agreement no 956090.
- Project link: https://apropos-project.eu/
- License: MIT Copyright (c) 2023 APROPOS

img src="https://raw.githubusercontent.com/aproposorg/KV260-PYNQ-tutorial/main/image/APROPOS.png" width="40" height="40">


- This work also within the scope of the Hardware aceleration for GNSS receiver project called Hard Sydr.



<img src="HWSYDR_SOURCELINK_VALID_AFTER_PUBLIC" width="40" height="40">



### The contributors of this tutorial are:
- Jie Lei, Universitat Politenica de Valencia, Spain
- Hans Jakob Damsgaard, Tampere University, Finland 
- Antoine Grenier, Tampere University, Finland 

### Source code may contain work from:

- ``Sydr Project`` https://github.com/aproposorg/sydr developed by Antoine Grenier.
- AMD Xilinx tutorial.


This design shows a simple PYNQ design that calculates correlation coefficients from two given signals. This function will be implemented in FPGA via Xilinx HLS, it called ''getCorrelator''.
 
 ---
## C function to be accelerated
This function is part of an open-source Software Defined Radio (SDR) for GNSS processing developed in Python, namely "Sydr" https://github.com/aproposorg/sydr developed by Antoine Grenier.

- The ''getCorrelator'' function can be viewed in https://github.com/aproposorg/sydr/blob/69d1ddf1f0caf7786600dcefa65ae81e2876a8c2/core/c_functions/tracking.c 

## Interface of the HLS design

This design use AXI Stream interface that supports dataflow type FPGA design to deliver fast and easy communication between the FPGA and host or within the FPGA itself. 

- The HLS design structure is based on the PYNQ tutorial written by Cathal McCabe from AMD.
https://github.com/cathalmccabe/PYNQ_tutorials/blob/master/hls_m_axi/hls_m_axi_tutorial.md

- To learn more about  AXI stream in Xilinx HLS, read Xilinx document ug1399 
https://docs.xilinx.com/r/2022.1-English/ug1399-vitis-hls/AXI-Adapter-Interface-Protocols

## Side Note before we start
  
- The implemented ``getCorrelator`` function in the HLS allow python to pass two ``double array[5]`` into the FPGA. 
	Then the FPGA will calculate its ``r_iCorr`` and ``r_qCorr``. These two values will be returned as an ``double return_array[5]``


- The AXI Stream example used in this design is simple to design and use in PYNQ. It allow Python to pass a buffer address to the FPGA, then following a control signal, the FPGA can start the calculation, then the result can be returned as buffer and arrives back to the python environment. 

- This tutorial will not optimize the HLS code since the optimization of C code for adapting HLS style can be designed depended.

- The design files: Vitis HLS project, Vivado, Bitstream will be open accessed. 

- The board we use here is Kria KV260 from AMD Xilinx. FPGA Part number: `XCK26-SFVC784-2LV-C`

# Setup C design golden reference

Our aim to to port a C function into HLS styled C in Vitis HLS. First, we need setuo golden reference by obtaining the C function output.

The C function will process the corrolation between two given signals namely ``iSignal`` and ``qSignal``. The calculation involve simple addtion and multiplication.

As we can see, two signal 
```c
double iSignal[5] = {-5.061244639011612, 9.39905932245856, 5.521358200447387, -3.904872069362163, 17.396453218094308};

double qSignal[5] = {41.46545312310088, 22.992557140364028, -13.982653668826686, -7.533390612593724, -18.637688038773916};

```

will produce correlation results

```c
 r_iCorr: 23.3508
 r_qCorr: 24.3043
```

We now can use this two value as reference to compare with the HLS design output.

You can see the result in SW_PYNQ/HLS_AXI_MASTER_TRACKING.ipynb matches with the golden reference.

In reality  this type of golden reference can be also done in the HLS project itself by execute the golden reference in the testbench of HLS project.
We will also cover that in the next section.

# Vitis HLS design

## Create a new project
Now we can use Vitis 2022.1 to create a new project for our board Xilinx KV260.

- This new board recentlly gained support in PYNQ, read more at https://github.com/Xilinx/Kria-PYNQ

Note: You may run into error when choosing the KV260 board in Vitis HLS. The error message is "Can not find part number". This is a known bug in Vitis HLS. The solution is to choose the part number directly instead of choosing the board.

So choose the part number XCK26-SFVC784-2LV-C or choose the SOM version of this part number from board list.

## Create interfaces
Since the C function we want to port is using double array, we need to create a double array interface in the HLS project. We can pass address of the array to the HLS function.

```c
void example(
    volatile double *IN_0, 
    volatile double *IN_1, 
    volatile double *OUT_Corr) {
}
```

We now need to tell the HLS that the IN_0 and IN_1 are input and OUT_Corr is output. We can do this by adding the following pragma to the function.To ask HLS to treat inputs and outputs as AXI Stream interfaces.

We also need to tell HLS the depth of the array. In this case, we have two array with 5 elements. So we need to set the depth to 5.

```c
#pragma HLS INTERFACE m_axi port=IN_0 depth=5 offset=slave 
#pragma HLS INTERFACE m_axi port=IN_1 depth=5 offset=slave 
#pragma HLS INTERFACE m_axi port=OUT_Corr depth=2 offset=slave 
```

Note we also need to enable control signal for the HLS function. It allow us to control the start and end of HLS kernel from python with a simple register overwirte. 
This is done by adding the following pragma.

```c
 #pragma HLS INTERFACE s_axilite port=return
```

Since the interface allow us to pass the array, we can use memory copy function to link the input with the internal variable. 

```c
  memcpy(iSignal,(  double*)IN_0, 5*sizeof(double));
  memcpy(qSignal,(  double*)IN_1, 5*sizeof(double));
  memcpy(out_cor,(  double*)OUT_Corr, 2*sizeof(double));

for (int i = 0; i < 5; i++){
    iSignal[i] = IN_0[i] ;
    qSignal[i] = IN_1[i] ;
    hls::print("i  %d \n", i);
    hls::print("The value of iSignal is %f \n", iSignal[i]);
    hls::print("The value of qSignal is %f \n\n", qSignal[i]);
    }  
```

Now we need to write the function for the ``getCorrelator`` by directing copying the C function into HLS. Some modification will be made, such as addping complex number header file, change some datatype that supported by HLS.

After adding the function, we can call it just like we call a function in the C programing. 

```c
getCorrelator(iSignal, qSignal, &r_iCorr, &r_qCorr);
```

After calculation, we obtain the results and save it 
```c
  memcpy((double *)OUT_Corr, OUT_Corr_buff, 2*sizeof(double));
```

## Testbench
We have a design written, now need a test bench for function verfication.

The testbench is very strightforward, create data and pass it to the HLS function. Then compare the result with the golden reference that simply run the C function.

We can compare the HLS result with C result, and printing visul hint to indicate the result is correct or not.

## Synthesis and Co-simulation

Now, start the C simulation to check for funcionality.
Synthesis and Co-simulation for detailed timing result and recomendation for optimization. 

The recommendations are important for delivering better performance of the design. 

You may see the pass message regarding the matching result 
```c
INFO: [Common 17-206] Exiting xsim at Sat Mar  4 22:42:04 2023...
INFO: [COSIM 212-316] Starting C post checking ...

============================
==   ^_^  FINE, NO DIFF   ==

    #######    ##    ## 
   ##     ##   ##   ##  
   ##     ##   ##  ##   
   ##     ##   #####    
   ##     ##   ##  ##   
   ##     ##   ##   ##  
    #######    ##    ## 
============================
INFO: [COSIM 212-1000] *** C/RTL co-simulation finished: PASS ***
``` 


## IP Export
Now we can export our design as IP into a Zip file.
Give it a specific name such ``IP_getCorrelator_2023``
You can inspect the zip file which contain some HDL files and configuration files

# Vivado block design

The purpose of Vivado design is to generate bitstream for our HLS design. The IP can not work alone, it need to be connected to the rest of the design such as ZYNQ controller, AXI interconnects for our stream interface.

In a simple design, this step is simple and straight forward. We can just drag and drop the IP into the block design and connect the interface.

## Create a new board design

Create a new board design for the KV260 board, and create a new block design. 

add the following IP into the block design
1) 1x ZYNQ Processing System
2) 2x AXI Interconnect
3) 1x interrupt controller
4) 1x system reset controller
5) 1x Your HLS IP

## Connect the IP

The connection can be done aotomatically by Vivado, but we need to guide the automation by made some manual connection.

```c
// From HLS IP to first AXI Interconnect
s_axi_control -> M00_AXI

// From HLS IP to SECOND AXI Interconnect
m_axi_gmem -> S00_AXI

// From first AXI Interconnect to ZYNQ 
S00_AXI -> M_AXI_HPM0_FPD
S01_AXI -> M_AXI_HPM1_FPD

// From second AXI Interconnect to ZYNQ 
M00_AXI -> S_AXI_HP0_FPD

// NOTE: Some port may not appear in your design, you can enable some port from the IP properties
// Such as S_AXI_HP0_FPD

// You may also need to connect all acknowledge signal together within a IP and interupt signal.
// You can see the attached image for the connection.
```
The connection will be 

<img src="VIVADO_BLOCK1_SOURCELINK_VALID_AFTER_PUBLIC" width="40" height="40">



## Generate bitstream

Make sure you validate the block design, and generate wrapper. 
Then generate the bitstream.

## Export the design

For the PYNQ workflow, you need to find the bitstream and hardware description file hwh. 
Simple way is to search the ``.bit`` and ``.hwh`` file in the project folder.
To copy it to the Jupiter enviroment in  PYNQ board.
The name of this two file need to be matched.


# PYNQ workflow

Now you can read the following python code to understand how to use the HLS IP in PYNQ board.
Visit the SW_PYNQ folder for the python code.



