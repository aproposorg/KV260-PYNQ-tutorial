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

#include <iostream>
#include <fstream>
#include <vector>
#include "hls_fft.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <cmath>
#include <string>

#define ROUNDING_PREC 3
using namespace std;

typedef complex<double> Complex;

int file_compare(string Infile1, string Infile2) {
	string fileName1 = Infile1;
	string fileName2 = Infile2;

	ifstream file1(fileName1);
	ifstream file2(fileName2);

	if (!file1.is_open() || !file2.is_open()) {
		cout << "Error opening files" << endl;
		return 1;
	}

	string line1, line2;
	int lineNumber = 0;
	int error = 0;
	while (getline(file1, line1) && getline(file2, line2)) {
		lineNumber++;
		if (line1 != line2) {
//                  cout << "Files differ at line " << lineNumber << ":" << endl;
//                  cout << fileName1 << " : " << line1 << endl;
//                  cout << fileName2 << " : " << line2 << endl;
			error += 1;
		}
	}

	if (error != 0) {
		cout << "\n\n !!! " << error << " mismatched  results";
		printf("\n\n=============================\n");
		cout << "   ##    ##   #######   \n";
		cout << "   ###   ##  ##     ##  \n";
		cout << "   ####  ##  ##     ##  \n";
		cout << "   ## ## ##  ##     ##  \n";
		cout << "   ##  ####  ##     ##  \n";
		cout << "   ##   ###  ##     ##  \n";
		cout << "   ##    ##   #######   \n";
		printf("=============================\n");
		file1.close();
		file2.close();
		return 1;
	} else {
		cout << "\n\nChecked  " << lineNumber << " line of results";
		printf("\n============================\n");
		printf("==   ^_^  FINE, NO DIFF   ==\n\n");
		cout << "    #######    ##    ## \n";
		cout << "   ##     ##   ##   ##  \n";
		cout << "   ##     ##   ##  ##   \n";
		cout << "   ##     ##   #####    \n";
		cout << "   ##     ##   ##  ##   \n";
		cout << "   ##     ##   ##   ##  \n";
		cout << "    #######    ##    ## \n";
		printf("=============================\n");
		file1.close();
		file2.close();
		return 0;
	}
}
void c_fft(vector<Complex> &input_signal_Cvar) {
	int size = input_signal_Cvar.size();
	if (size == 1) {
		return;
	}

	vector<Complex> even(size / 2), odd(size / 2);
	for (int i = 0; i < size / 2; i++) {
		even[i] = input_signal_Cvar[2 * i];
		odd[i] = input_signal_Cvar[2 * i + 1];
	}

	c_fft(even);
	c_fft(odd);

	for (int i = 0; i < size / 2; i++) {
		Complex w = polar(1.0, -2 * M_PI * i / size);
		input_signal_Cvar[i] = even[i] + w * odd[i];
		input_signal_Cvar[i + size / 2] = even[i] - w * odd[i];
	}
}

// https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Writing-a-Test-Bench
int main() {
	// Set up to run FFT by calling HLS top function
	// Read input file from the file, process for FFT
	// Change `nfft` acording to the total number of elements in your input file
	// To change the configuration of your FFT, modify the hls_fft.h file where the parameters of Xilinx hls_IP was defined
	fstream fin("sim_sample_small.dat");
	int nfft = 10; //2^nfft = total number of data point in your input file

	vector<float> vec_data;
	float num;
	int data_size = 1 << nfft; // Perform 32-point FFT
	for (int i = 0; i < data_size; i++) {
		fin >> num;
		vec_data.push_back(num);
	}

	// Writing FFT sample data to the stream
	fft_stream c_input_ref_data, output_data;
	for (int i = 0; i < data_size; i++) {
		data_t in_data;
		in_data.data = vec_data[i];
		in_data.last = (i == data_size - 1);
		c_input_ref_data.write(in_data);
	}

	// FFT Execution from HLS
	hls_fft(c_input_ref_data, output_data, nfft);

	ofstream HLS_outfile("HLS_output.txt");
	if (HLS_outfile.is_open()) {
		cout << "\n\n\nThe HLS design finished, saving data into file \n";
		data_t dout;
		do {
			dout = output_data.read();
			HLS_outfile << fixed << setprecision(ROUNDING_PREC) << dout.data << "\n";
		} while (!dout.last);
	} else {
		cout << "Error: could not open output file" << endl;
		return 1;
	}



	// Set up C based FFT, using the same input data to generate FFT results using standard C++
	// compute and saving data into file to be used to compare with HLS output
	// The data type used in HLS and C++ is slighly difference, so the results will not macth complectlly,
		// Thus we round the output
	ifstream input_signal_from_file("sim_sample_small.dat");
	if (!input_signal_from_file) {
		cerr << "Error: could not open input_signal_Cvar file" << endl;
		return 1;
	}
	vector<Complex> input_signal_Cvar;
	double value;
	while (input_signal_from_file >> value) {
		input_signal_Cvar.push_back( { value, 0 });
	}
	input_signal_from_file.close();
	int size = input_signal_Cvar.size();
	int padded_size = pow(2, ceil(log2(size)));
	input_signal_Cvar.resize(padded_size, { 0, 0 });

	c_fft(input_signal_Cvar);

	ofstream Cpp_outfile("CREF_output.txt");
	if (Cpp_outfile.is_open()) {
		cout << "\nThe C reference design finished, saving result into file \n";
		for (int i = 0; i < size; i++) {
//			cout << input_signal_Cvar[i] << ", ";
			Cpp_outfile  << fixed << setprecision(ROUNDING_PREC) << input_signal_Cvar[i]  << "\n";
		}
	} else {
		cout << "Error: could not open output file" << endl;
		return 1;
	}
	cout << "\n \n";
	Cpp_outfile.close();
	HLS_outfile.close();



	/// Checking results of HW and SW, are they matched?
	// Compare each line of the data
	string fileName1 = "HLS_output.txt";
	string fileName2 = "CREF_output.txt";
	file_compare (fileName1, fileName2);
}

