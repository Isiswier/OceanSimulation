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

	struct HDNinfo {
		complex h;			// wave height
		vector2 D;			// displacement
		vector3 n;			// normal
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
	
	void dispHeightToFile();
	void evaluateWaves(float t);
	void evaluateWavesFFT(float t);

	float dispersion(int n_prime, int m_prime);
	float phillips  (int n_prime, int m_prime);
	
	complex hTilde_0(int n_prime, int m_prime);
	complex hTilde  (float t, int n_prime, int m_prime);
	HDNinfo h_D_and_n(vector2 x, float t);

private:
	friend class MeshPlane;						// MeshPlane accesses Ocean::vertices
	const float g;
	
	int			N, Nplus1;	                    // dimension -- N should be a power of 2
	float		A, length;                      // A: phillips spectrum parameter -- affects heights of waves
	vector2		w;                              // wind paramete
	complex		*h_tilde = 0,                   // for fast fourier transform
				*h_tilde_slopex = 0,
				*h_tilde_slopez = 0,
				*h_tilde_dx = 0,
				*h_tilde_dz = 0;
	cFFT		*fft = 0;

	Vertex		 *vertices = 0;	
};

#endif