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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <stdbool.h>
#include <assert.h>

#include <stdio.h>
#include <string.h>

#include "source1.h"

// This is the function that will be called from the PL side
// The getCorrelator function is the function that will be synthesized
void getCorrelator(double* iSignal,
                   double* qSignal,
                   int* code,
                   int  size, // samplesRequired
                   double codeStep,
                   double remCodePhase,
                   double correlatorSpacing,
                   double* r_iCorr,
                   double* r_qCorr)
{
    unsigned int  codeIdx = 0;

    double start = remCodePhase + correlatorSpacing;
    double stop = size * codeStep + remCodePhase + correlatorSpacing;
    double step = (stop - start) / size;

    *r_iCorr = 0.0;
    *r_qCorr = 0.0;
    for (unsigned int  idx=0; idx < size; idx++)
    {
        codeIdx = ceil(start + step * idx);

        *r_iCorr += code[codeIdx] * iSignal[idx];
        *r_qCorr += code[codeIdx] * qSignal[idx];
    }
    return;
}


// top level function for HLS synthesis
void example(volatile double *IN_0, volatile double *IN_1, volatile double *OUT_Corr)
{

// The inputs contain 5 value each, so depth is 5
#pragma HLS INTERFACE m_axi port=IN_0 depth=5 offset=slave 
#pragma HLS INTERFACE m_axi port=IN_1 depth=5 offset=slave 
#pragma HLS INTERFACE m_axi port=OUT_Corr depth=2 offset=slave 
 // this will enable the control signals for the PL design to be started from PYNQ notebook
#pragma HLS INTERFACE s_axilite port=return

 // declear all the variables needed for the test
 // These variable can be passed to the getCorrelator function also via stream access, 
    // but modification of top level function is required
int size = 5;
double iSignal[5] = {0};
double qSignal[5] = {0};
double out_cor[2] = {0};
int code[5] = {-1, 1, 1, 1, -1};
double codeStep = 0.1023;
double remCodePhase = 0.0;
double correlatorSpacing = 0.5;
double r_iCorr = 0.0;
double r_qCorr = 0.0;

//memcpy creates a burst access to memory
  //multiple calls of memcpy cannot be pipelined and will be scheduled sequentially
  //memcpy requires a local buffer to store the results of the memory transaction
memcpy(iSignal,(  double*)IN_0, 5*sizeof(double));
memcpy(qSignal,(  double*)IN_1, 5*sizeof(double));
memcpy(out_cor,(  double*)OUT_Corr, 2*sizeof(double));

for (int i = 0; i < 5; i++)
    {
        iSignal[i] = IN_0[i] ;
        qSignal[i] = IN_1[i] ;
        hls::print("i  %d \n", i);
        hls::print("The value of iSignal is %f \n", iSignal[i]);
        hls::print("The value of qSignal is %f \n\n", qSignal[i]);
    }  

getCorrelator(iSignal, qSignal, code, size, codeStep, 
                remCodePhase, correlatorSpacing, &r_iCorr, &r_qCorr);

double OUT_Corr_buff[2] = {0};
OUT_Corr_buff[0] = r_iCorr;
OUT_Corr_buff[1] = r_qCorr;

#ifdef HLS_DEBUF_PRINT
    hls::print("After calling getCorrelator \n");
    hls::print("The value of r_iCorr is %f \n", r_iCorr);
    hls::print("The value of r_qCorr is %f \n", r_qCorr);    

    hls::print("The value of OUT_Corr_buff 0 is %f \n", OUT_Corr_buff[0]);
    hls::print("The value of OUT_Corr_buff 1 is %f \n", OUT_Corr_buff[1]);    
#endif


memcpy((double *)OUT_Corr, OUT_Corr_buff, 2*sizeof(double));

}
