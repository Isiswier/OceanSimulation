#include "Ocean.h"
#include "misc.h"
#include <cstdio>


Ocean::Ocean(int n, float A, vector2 w, float length) : g(9.81), N(n), A(A), w(w), length(length)
{
	mfft		   = new FFT(N);
	h_tilde		   = new complex[N*N];
	h_tilde_dx	   = new complex[N*N];
	h_tilde_dz	   = new complex[N*N];
	h_tilde_slopex = new complex[N*N];
	h_tilde_slopez = new complex[N*N];


	int index;
	vertices = new Vertex[N*N];
	
	complex htilde0, htilde0mk_conj;
	srand(time(NULL));

	for (int row = 0; row < N; row++) {
		for (int col = 0; col < N; col++) {

			htilde0 = hTilde_0(col, row);
			htilde0mk_conj = hTilde_0(-col, -row).conj();

			index = row * N + col;
			vertices[index].a = htilde0.a;
			vertices[index].b = htilde0.b;
			vertices[index]._a = htilde0mk_conj.a;
			vertices[index]._b = htilde0mk_conj.b;

			vertices[index].ox = vertices[index].x = (col - (N >> 1)) * length / N;
			vertices[index].oy = vertices[index].y = 0.0f;
			vertices[index].oz = vertices[index].z = (row - (N >> 1)) * length / N;

			vertices[index].nx = 0.0f;
			vertices[index].ny = 1.0f;
			vertices[index].nz = 0.0f;
		}
	}
}

Ocean::~Ocean() {
	if (h_tilde)        delete[] h_tilde;
	if (h_tilde_slopex) delete[] h_tilde_slopex;
	if (h_tilde_slopez) delete[] h_tilde_slopez;
	if (h_tilde_dx)     delete[] h_tilde_dx;
	if (h_tilde_dz)     delete[] h_tilde_dz;
	if (vertices)       delete[] vertices;
	if (mfft)			delete mfft;
}

float Ocean::dispersion(int col, int row) {
	float w_0 = 2.0f * M_PI / 200.0f;
	float kx = M_PI * (2 * col - N) / length;
	float kz = M_PI * (2 * row - N) / length;
	return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}

float Ocean::phillips(int col, int row) {
	vector2 k(M_PI * (2 * col - N) / length,
		M_PI * (2 * row - N) / length);
	float k_length = k.length();
	if (k_length < 0.000001) return 0.0;

	float k_length2 = k_length  * k_length;
	float k_length4 = k_length2 * k_length2;

	float k_dot_w = k.unit() * w.unit();
	float k_dot_w2 = k_dot_w * k_dot_w;

	float w_length = w.length();
	float L = w_length * w_length / g;
	float L2 = L * L;

	float damping = 0.001;
	float l2 = L2 * damping * damping;

	return A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
}

complex Ocean::hTilde_0(int col, int row) {
	complex r = gaussianRandomVariable();
	return r * sqrt(phillips(col, row) / 2.0f);
}

complex Ocean::hTilde(float t, int col, int row) {
	int index = row * N + col;

	complex htilde0(vertices[index].a, vertices[index].b);
	complex htilde0mkconj(vertices[index]._a, vertices[index]._b);

	float omegat = dispersion(col, row) * t;

	float cos_ = cos(omegat);
	float sin_ = sin(omegat);

	complex c0(cos_, sin_);
	complex c1(cos_, -sin_);

	complex res = htilde0 * c0 + htilde0mkconj * c1;

	return htilde0 * c0 + htilde0mkconj*c1;
}


void Ocean::evalWavesCPUFFT(float t)
{
	float kx, kz, len, lambda = -0.5f;
	int index, index1;


	for (int row = 0; row < N; row++) {
		kz = M_PI * (2.0f * row - N) / length;
		for (int col = 0; col < N; col++) {
			kx = M_PI*(2 * col - N) / length;
			len = sqrt(kx * kx + kz * kz);
			index = row * N + col;

			h_tilde[index] = hTilde(t, col, row);
			h_tilde_slopex[index] = h_tilde[index] * complex(0, kx);
			h_tilde_slopez[index] = h_tilde[index] * complex(0, kz);
			if (len < 0.000001f) {
				h_tilde_dx[index] = complex(0.0f, 0.0f);
				h_tilde_dz[index] = complex(0.0f, 0.0f);
			}
			else {
				h_tilde_dx[index] = h_tilde[index] * complex(0, kx / len);
				h_tilde_dz[index] = h_tilde[index] * complex(0, kz / len);
			}
		}
	}

	mfft->cpufft2(h_tilde);
	mfft->cpufft2(h_tilde_slopex);
	mfft->cpufft2(h_tilde_slopez);
	mfft->cpufft2(h_tilde_dx);
	mfft->cpufft2(h_tilde_dz);

#ifdef DEBUGFFT
	FILE* fftFile = fopen(FFT_FILE00, "w");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			fprintf(fftFile, "%f ", h_tilde[i * N + j].a);
		fprintf(fftFile, "\n");
	}
	fclose(fftFile);
#endif


	vector3 n;
	int signs[] = { 1, -1 };
	int sign;
	for (int row = 0; row < N; row++) {
		for (int col = 0; col < N; col++) {
			index = row * N + col;
			index1 = row * N + col;	// index into vertices

			sign = signs[(col + row) & 1];

			h_tilde[index] = h_tilde[index] * sign;
			
			// height
			vertices[index1].y = h_tilde[index].a;

			// displacement
			h_tilde_dx[index] = h_tilde_dx[index] * sign;
			h_tilde_dz[index] = h_tilde_dz[index] * sign;
			vertices[index1].x = vertices[index1].ox + h_tilde_dx[index].a * lambda;
			vertices[index1].z = vertices[index1].oz + h_tilde_dz[index].a * lambda;

			// normal
			h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
			h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
			n = vector3(0.0f - h_tilde_slopex[index].a, 1.0f, 0.0f - h_tilde_slopez[index].a).unit();

			vertices[index1].nx = n.x;
			vertices[index1].ny = n.y;
			vertices[index1].nz = n.z;

		}
	}
}

void Ocean::evalWavesGPUFFT(float t)
{
	float kx, kz, len, lambda = -0.5f;
	int index, index1;


	for (int row = 0; row < N; row++) {
		kz = M_PI * (2.0f * row - N) / length;
		for (int col = 0; col < N; col++) {
			kx = M_PI*(2 * col - N) / length;
			len = sqrt(kx * kx + kz * kz);
			index = row * N + col;

			h_tilde[index] = hTilde(t, col, row);
			h_tilde_slopex[index] = h_tilde[index] * complex(0, kx);
			h_tilde_slopez[index] = h_tilde[index] * complex(0, kz);
			if (len < 0.000001f) {
				h_tilde_dx[index] = complex(0.0f, 0.0f);
				h_tilde_dz[index] = complex(0.0f, 0.0f);
			}
			else {
				h_tilde_dx[index] = h_tilde[index] * complex(0, kx / len);
				h_tilde_dz[index] = h_tilde[index] * complex(0, kz / len);
			}
		}
	}

	mfft->gpufft2(h_tilde);
	mfft->gpufft2(h_tilde_slopex);
	mfft->gpufft2(h_tilde_slopez);
	mfft->gpufft2(h_tilde_dx);
	mfft->gpufft2(h_tilde_dz);

#ifdef DEBUGFFT
	FILE* fftFile = fopen(FFT_FILE00, "w");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			fprintf(fftFile, "%f ", h_tilde[i * N + j].a);
		fprintf(fftFile, "\n");
	}
	fclose(fftFile);
#endif


	vector3 n;
	int signs[] = { 1, -1 };
	int sign;
	for (int row = 0; row < N; row++) {
		for (int col = 0; col < N; col++) {
			index = row * N + col;
			index1 = row * N + col;	// index into vertices

			sign = signs[(col + row) & 1];

			h_tilde[index] = h_tilde[index] * sign;

			// height
			vertices[index1].y = h_tilde[index].a;

			// displacement
			h_tilde_dx[index] = h_tilde_dx[index] * sign;
			h_tilde_dz[index] = h_tilde_dz[index] * sign;
			vertices[index1].x = vertices[index1].ox + h_tilde_dx[index].a * lambda;
			vertices[index1].z = vertices[index1].oz + h_tilde_dz[index].a * lambda;

			// normal
			h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
			h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
			n = vector3(0.0f - h_tilde_slopex[index].a, 1.0f, 0.0f - h_tilde_slopez[index].a).unit();

			vertices[index1].nx = n.x;
			vertices[index1].ny = n.y;
			vertices[index1].nz = n.z;

		}
	}
}





// Helper Function
 void Ocean::dispHtildeToFile()
{
	FILE* ff = fopen(H_FILE, "w");
	fprintf(ff, "HTILDE0\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			fprintf(ff, "(%f, %f) ", vertices[i * N + j].a, vertices[i * N + j].b);
		fprintf(ff, "\n");
	}

	fprintf(ff, "\n HTILDE\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			fprintf(ff, "(%f, %f) ", h_tilde[i * N + j].a, h_tilde[i * N + j].b);
		fprintf(ff, "\n");
	}
	fclose(ff);
}