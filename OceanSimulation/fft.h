#ifndef _FFT_H
#define _FFT_H

#include <cmath>
#include "misc.h"
#include "defs.h"

typedef unsigned int UINT;

/*************************************
 * We suppose that rows == cols == N
 *************************************
 */

class FFT {
public:
	FFT(UINT N);
	~FFT();

	inline UINT reverse(UINT x) {
		UINT n = 0;
		for (int i = 0; i < log2N; i++) {
			n <<= 1;
			n |= (x & 1);
			x >>= 1;
		}
		return n;
	}

	void cpufft2(complex* in);
	void gpufft2(complex* in);


private:
	UINT	N;
	UINT	log2N;
	UINT	memSize;

	complex*	  temp = 0;
	cufftHandle   plan;
	cufftComplex* deviceData;

	void fftRow(complex* in);
	void fftCol(complex* in);
};

#endif