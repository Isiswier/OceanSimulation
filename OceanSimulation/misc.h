#ifndef _MISC_H_
#define _MISC_H_

#include "complex.h"
#include <ctime>
#include <cuda_runtime.h>
#include <cufft.h>
#include <helper_functions.h>
#include <helper_cuda.h>

float uniformRandomVariable();
complex gaussianRandomVariable();

#endif