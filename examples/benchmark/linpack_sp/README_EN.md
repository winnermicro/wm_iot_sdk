# Linpack

## Overview

Linpack is used to test floating-point performance.

## Requirements

None.

## Building and flashing

Example Location： `examples/benchmark/linpack_sp`

For compiling, burning, and others, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running result

If it runs successfully, it will output the following logs.

```
Unrolled Single Precision Linpack

Unrolled Single Precision Linpack

     norm. resid      resid           machep         x[0]-1        x[n-1]-1
       2.2                     e               e               e               e
    times are reported for matrices of order   120
      dgefa      dgesl      total       kflops     unit      ratio
 times for array with leading dimension of  121
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      33834       0.06       0.62
 times for array with leading dimension of 120
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      34729       0.06       0.61
       0.03       0.00       0.03      33834       0.06       0.62
Unrolled Single  Precision 33834 Kflops ; 10 Reps
```