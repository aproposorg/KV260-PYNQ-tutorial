
// This file obtained from
// https://github.com/aproposorg/sydr/blob/69d1ddf1f0caf7786600dcefa65ae81e2876a8c2/core/c_functions/tracking.c
// commit 69d1ddf1f0caf7786600dcefa65ae81e2876a8c2

/*
 * MIT License

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <stdbool.h>
#include <assert.h>


#include "tracking.h"
// GPS' definition of Pi
#define PI 3.1415926535898


int main(){
    printf("Hello World!\n");


    int size = 5;
    double iSignal[5] = {-5.061244639011612, 9.39905932245856, 5.521358200447387, -3.904872069362163, 17.396453218094308};
    double qSignal[5] = {41.46545312310088, 22.992557140364028, -13.982653668826686, -7.533390612593724, -18.637688038773916};
    int code[5] = {-1, 1, 1, 1, -1};
    double codeStep = 0.1023;
    double remCodePhase = 0.0;
    double correlatorSpacing = 0.5;
    double r_iCorr = 0.0;
    double r_qCorr = 0.0;

    getCorrelator(iSignal, qSignal, code, size, codeStep, remCodePhase, correlatorSpacing, &r_iCorr, &r_qCorr);

    printf( "remCodePhase: %.4f \n correlatorSpacing: %.4f\n &r_iCorr: %.4f\n &r_qCorr: %.4f\n "
            ,remCodePhase, correlatorSpacing, r_iCorr, r_qCorr);
}

