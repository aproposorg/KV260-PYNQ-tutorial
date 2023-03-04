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
/*
@brief Perform a correlation operation between the code and I/Q signal.
@param iSignal           Array with the real part of the signal.
@param qSignal           Array with the imaginary part of the signal.
@param code              Array with the PRN code.
@param size              Size of the arrays (iSignal, qSignal, code, iCorr, qCorr).
@param codeStep          The step size between the code frequency and the sampling frequency.
@param remCodePhase      Remaining code phase from last loop.
@param correlatorSpacing Correlator spacing in code chip.
@param r_iCorr             Array with correlation result for the real part.
@param r_qCorr             Array with correlation result for the imaginary part.
@return void.
*/

#include "tracking.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <stdbool.h>
#include <assert.h>

void getCorrelator(double* iSignal,
                   double* qSignal,
                   int* code,
                   int size, // samplesRequired
                   double codeStep,
                   double remCodePhase,
                   double correlatorSpacing,
                   double* r_iCorr,
                   double* r_qCorr)
{
    int codeIdx = 0;

    double start = remCodePhase + correlatorSpacing;
    double stop = size * codeStep + remCodePhase + correlatorSpacing;
    double step = (stop - start) / size;

    *r_iCorr = 0.0;
    *r_qCorr = 0.0;
    for (int idx=0; idx < size; idx++)
    {
        codeIdx = ceil(start + step * idx);

        *r_iCorr += code[codeIdx] * iSignal[idx];
        *r_qCorr += code[codeIdx] * qSignal[idx];
    }

    return;
}

// --------------------------------------------------------------------------------------------------------------------
