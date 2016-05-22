#include "fft.h"

/**************************************************
 * Assert: sizeof(complex) == sizeof(cufftComplex)
 **************************************************
 */


void FFT::gpufft2(complex* in)
{
	// Copy host memory to device
	cudaMemcpy(deviceData, in, memSize, cudaMemcpyHostToDevice);


	// Transform 
	checkCudaErrors(cufftExecC2C(plan, (cufftComplex *)deviceData, (cufftComplex *)deviceData, CUFFT_FORWARD));


	// Check if kernel execution generated and error
	//getLastCudaError("Kernel execution failed");


	// Copy device memory to host
	checkCudaErrors(cudaMemcpy(in, deviceData, memSize, cudaMemcpyDeviceToHost));

}