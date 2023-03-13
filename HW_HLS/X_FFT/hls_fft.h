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
#pragma once

#include <hls_fft.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>

// The type of input/output can be either float complex or float.
// Uncomment out when using float.
// #define USE_FLOAT

const unsigned TWO_TO_THE_POWER_OF_N_MAX = 10;
const unsigned FFT_LENGTH_MAX = 1 << TWO_TO_THE_POWER_OF_N_MAX;

// definition of the parameters https://docs.xilinx.com/r/2022.1-English/ug1399-vitis-hls/FFT-Static-Parameters
// set the individual parameter for the Xilinx FFT IP
struct fft_param: hls::ip_fft::params_t {
	static const bool has_nfft = true;
	static const bool ovflo = false;
//	static const unsigned input_width = 32;
//	static const unsigned output_width = 32;

	static const unsigned input_width = 32;
	static const unsigned output_width = 32;

	static const unsigned status_width = 8;
	static const unsigned config_width = 24;
	static const unsigned phase_factor_width = 24;
	static const unsigned stages_block_ram =
			(TWO_TO_THE_POWER_OF_N_MAX < 10) ?
					0 : (TWO_TO_THE_POWER_OF_N_MAX - 9);
	static const unsigned max_nfft = TWO_TO_THE_POWER_OF_N_MAX;
	static const unsigned arch_opt = hls::ip_fft::pipelined_streaming_io;
	static const unsigned ordering_opt = hls::ip_fft::natural_order;
	static const unsigned scaling_opt = hls::ip_fft::scaled;
	static const unsigned rounding_opt = hls::ip_fft::truncation;
};

typedef hls::ip_fft::config_t<fft_param> config_t;
typedef hls::ip_fft::status_t<fft_param> status_t;
#if defined(USE_FLOAT)
typedef hls::axis<float, 0, 0, 0> data_t;
#else
// set the data type of the data in the design
typedef hls::axis<std::complex<float>, 0, 0, 0> data_t;
#endif
// define a stream interface as `fft_stream`
typedef hls::stream<data_t> fft_stream;

// Top function
void hls_fft(fft_stream &stream_input, fft_stream &stream_output,
		unsigned size);
