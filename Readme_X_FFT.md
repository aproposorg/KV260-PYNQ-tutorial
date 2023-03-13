# Implementing Xilinx FFT library in PYNQ

## Description
This tutorial shows the steps of using Xilinx FFT IP with DMA and stream interface. The FFT IP is used to calculate the FFT of a given signal. The FFT IP is implemented in Vivado HLS, and the Vivado block design is created to connect the IP with DMA and stream interface. The overlay is created and used in python environment.

To read more about the Xilinx FFT library see [Xilinx FFT Library](https://docs.xilinx.com/r/2022.1-English/ug1399-vitis-hls/FFT-IP-Library
).

This work contain code from https://github.com/DYGV/HLS_FFT

## Setup C design golden reference

This design will include the C reference design into the testbench. It will read in input from a file, then process for FFT results. See [testbench](https://github.com/aproposorg/KV260-PYNQ-tutorial/blob/ba37a4d29cfbfc12a2c566d68905f507378146bd/HW_HLS/X_FFT/tb_source.cpp#L96)
 for more details.

## Vitis HLS design

The interface of this design will take in a stream interface for the input signal then return the FFT value from stream. In addition, the size of FFT problem can be controlled from a register write using `s_axilite` interface.

```cpp
void hls_fft(fft_stream &stream_input, fft_stream &stream_output,
		unsigned size) {

#pragma HLS INTERFACE axis port = stream_input
#pragma HLS INTERFACE axis port = stream_output
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return
```

The design use complex number buffer to contain the input and output data. The complex number is defined as

```cpp
	std::complex<float> fft_in[FFT_LENGTH_MAX] __attribute__((no_ctor));
```

The FFT library can be called from 
```cpp
	hls::fft<fft_param>(fft_input, fft_output, &fft_status, &fft_config);
```
Note, we need to specify the parameter to configure the IP, read more detailed parameters from [FFT Static Parameters](https://docs.xilinx.com/r/2022.1-English/ug1399-vitis-hls/FFT-Static-Parameters
).
We defined them in https://github.com/aproposorg/KV260-PYNQ-tutorial/blob/main/HW_HLS/X_FFT/hls_fft.h


Now we can move on to the [testbench](https://github.com/aproposorg/KV260-PYNQ-tutorial/blob/main/HW_HLS/X_FFT/tb_source.cpp)

It calls the HLS design first and saves the output, then it calls the C reference design and compare the results. Note here we are rounding the results to 3 digits after the decimal, since the precision of the C reference design is slightlly different with the HLS. 


# Vivado block design

Similar with the previous tutorial, we need to create a Vivado block design to connect the IP with DMA and stream interface.
We need manually enable some interface of the Zynq block such as master interface `S_AXI_HP0`. The DMA memory map width also need to increased to 64 to match with the stream interface of your IP.
The block design is shown below.

<img src="https://raw.githubusercontent.com/aproposorg/KV260-PYNQ-tutorial/main/image/X_FFT_BOARD.png" >


# Create the overlay

This step similar with the previous tutorial. 

Note here we are using DMA for data transfer, so we use a API to call for transfer and receive. We also need to pass a scarlar into the IP which control the size of FFT problem. 

```cpp
    dma_ip.sendchannel.transfer(in_buffer)
    dma_ip.recvchannel.transfer(out_buffer)
    fft_ip.write(0x00, 0x01)
```
