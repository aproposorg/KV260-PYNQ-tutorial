/*
 MIT License
 Copyright (c) 2023 APROPOS

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "hls_print.h"
#include <complex>
#include "hls_fft.h"
// https://docs.xilinx.com/r/2022.1-English/ug1399-vitis-hls/FFT-IP-Library

// stream in data and load the `data` into stream
void read_input_data_from_stream(fft_stream &stream_input,
		std::complex<float> fft_input[FFT_LENGTH_MAX]) {

	data_t data_in;
	int i = 0;
	do {
// A pipelined function or loop can process new inputs every <N> clock cycles,
//where <N> is the II of the loop or function. An II of 1 processes a new input every clock cycle.
#pragma HLS PIPELINE II = 1
		data_in = stream_input.read();
		fft_input[i++] = data_in.data;
	} while (!data_in.last);
}

void call_fft_ip(std::complex<float> fft_input[FFT_LENGTH_MAX],
		std::complex<float> fft_output[FFT_LENGTH_MAX], unsigned size) {
	config_t fft_config;
	status_t fft_status;
	fft_config.setDir(1);
	fft_config.setNfft(size);
	// This is XILINX FFT Library
	hls::fft<fft_param>(fft_input, fft_output, &fft_status, &fft_config);
}

// write the `data` of fft output into the stream
void write_output_data_to_stream(fft_stream &stream_output,
		std::complex<float> fft_output[FFT_LENGTH_MAX], unsigned size) {
	data_t data_out;
	data_out.keep = -1;
	data_out.strb = -1;

	for (int i = 0; i < size; i++) {
#if defined(USE_FLOAT)
        #pragma HLS PIPELINE II = 2
        std::complex<float> data = fft_output[i];
        data_out.data = data.real();
        data_out.last = 0;
        stream_output.write(data_out);
        data_out.data = data.imag();
        data_out.last = (i == size - 1);
        stream_output.write(data_out);
#else
#pragma HLS PIPELINE II = 1
		data_out.data = fft_output[i];
		data_out.last = (i == size - 1);
		stream_output.write(data_out);
#endif
	}
}

// top level function for taking inputs as stream
// process for FFT
// function defined input and output stream as axis port.
// taking one scalar `size` in using `s_axilite`, this interface commonly used for simple arguments inputs outputs
void hls_fft(fft_stream &stream_input, fft_stream &stream_output,
		unsigned size) {

#pragma HLS INTERFACE axis port = stream_input
#pragma HLS INTERFACE axis port = stream_output
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return

	// define the buffer needed for FFT function
	std::complex<float> fft_in[FFT_LENGTH_MAX] __attribute__((no_ctor));
	std::complex<float> fft_out[FFT_LENGTH_MAX] __attribute__((no_ctor));
	// set these input and output as stream interface
#pragma HLS STREAM variable = fft_in
#pragma HLS STREAM variable = fft_out
	// ask to optimise as dataflow model
#pragma HLS dataflow
	// FFT point must be at least 8(2^3) points
	if (size < 3) {
		// If size is smaller than 3, FFT is performed with the maximum value
		size = TWO_TO_THE_POWER_OF_N_MAX;
	}
	// extract input stream and load the data into the buffer `fft_in`
	read_input_data_from_stream(stream_input, fft_in);
	// calculate and load the data back to buffer `fft_out`
	call_fft_ip(fft_in, fft_out, size);
	// stream out the data
	write_output_data_to_stream(stream_output, fft_out, (1 << size));
}
