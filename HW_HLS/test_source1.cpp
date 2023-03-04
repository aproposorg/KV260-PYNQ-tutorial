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
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "source1.h"
#include <stdio.h>

using namespace std;

// /////////////////////
void getCorrelator(double*  ,
                   double*  ,
                   int*  ,
                   int   , 
                   double  ,
                   double  ,
                   double  ,
                   double*  ,
                   double*  );
/////////////////////


void example(volatile double *IN_0, volatile double *IN_1, volatile double *OUT_Corr);

int main()
{ 
 
    double iSignal_tb[5] = {0};
    double qSignal_tb[5] = {0};
    double out_tb[2] = {0};

   for (unsigned i = 0; i < 5; i++) {
        iSignal_tb[i] = i+1;
        qSignal_tb[i] = (i+1)*10;   
    }

// we call the PL function here, this line will be executed in HW Simulator
    example(iSignal_tb, qSignal_tb, out_tb);    

    int size = 5;
    int code[5] = {-1, 1, 1, 1, -1};
    double codeStep = 0.1023;
    double remCodePhase = 0.0;
    double correlatorSpacing = 0.5;
    double r_iCorr = 0.0;
    double r_qCorr = 0.0;

 // Note here, we call the C function from testbench, this line will be executed in SW C simluator
    getCorrelator(iSignal_tb, qSignal_tb, code, size, codeStep, 
                  remCodePhase, correlatorSpacing, &r_iCorr, &r_qCorr);

// result comparison from HW and SW simulation, 
// out_tb is the result from HW simulation,
// r_iCorr and r_qCorr are the result from SW simulation
    if ( (out_tb[0] != r_iCorr) || (out_tb[1] != r_qCorr) ) {
        std::cout << "ERROR: results mismatch" << std::endl;
        printf("ERROR HW and SW results mismatch\n");
        printf("\n\n=============================\n");
            printf("\n\n=============================\n");
            std::cout << "   ##    ##   #######   \n";
            std::cout << "   ###   ##  ##     ##  \n";
            std::cout << "   ####  ##  ##     ##  \n";
            std::cout << "   ## ## ##  ##     ##  \n";
            std::cout << "   ##  ####  ##     ##  \n";
            std::cout << "   ##   ###  ##     ##  \n";
            std::cout << "   ##    ##   #######   \n";
            printf("=============================\n");
            return 1;
    }
    else {
        printf("\n\n============================\n");
        printf("==   ^_^  FINE, NO DIFF   ==\n\n");
        std::cout << "    #######    ##    ## \n";
        std::cout << "   ##     ##   ##   ##  \n";
        std::cout << "   ##     ##   ##  ##   \n";
        std::cout << "   ##     ##   #####    \n";
        std::cout << "   ##     ##   ##  ##   \n";
        std::cout << "   ##     ##   ##   ##  \n";
        std::cout << "    #######    ##    ## \n";
        printf("============================\n");
        return 0;

        }
    }

