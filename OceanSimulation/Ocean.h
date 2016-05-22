#ifndef _OCEAN_H
#define _OCEAN_H

#include "defs.h"
#include "vector.h"
#include "fft.h"

class Ocean {
public:
	struct Vertex {
		float   x, y, z;	// vertex position
		float  nx, ny, nz;	// vertex normal
		float   a, b, c;	// htilde_0
		float  _a, _b, _c;	// htilde0mk conjugate
		float  ox, oy, oz;	// original position
	};

	struct OceanPara {
		OceanPara(float inA = 0.004f, vector2 inW = vector2(20.0f, 20.0f)) {
			A = inA;	w = inW;
		}
		float	A;
		vector2 w;
	};

	Ocean(int N, float A, vector2 w, float length);
	~Ocean();
	
	void dispHtildeToFile();
	void evaluateWavesFFT(float t);
	void evalWavesCPUFFT(float t);
	void evalWavesGPUFFT(float t);

	float dispersion(int col, int row);
	float phillips  (int col, int row);
	
	complex hTilde_0(int col, int row);
	complex hTilde  (float t, int col, int row);

private:
	friend class MeshPlane;						// MeshPlane accesses Ocean::vertices
	const float g;
	
	int			N;			                    // dimension -- N should be a power of 2
	float		A, length;                      // A: phillips spectrum parameter -- affects heights of waves
	vector2		w;                              // wind paramete
	complex		*h_tilde = 0,                   // for fast fourier transform
				*h_tilde_slopex = 0,
				*h_tilde_slopez = 0,
				*h_tilde_dx = 0,
				*h_tilde_dz = 0;
	FFT			*mfft = 0;

	Vertex		 *vertices = 0;	
};

#endif