#include "misc.h"
#include <cmath>
#include <cstdlib>
#include <random>

float uniformRandomVariable() {
	return (float)rand() / RAND_MAX;
}

complex gaussianRandomVariable() {
	float x1, x2, w;
	do {
		x1 = 2.f * uniformRandomVariable() - 1.f;
		x2 = 2.f * uniformRandomVariable() - 1.f;
		w = x1 * x1 + x2 * x2;
	} while (w >= 1.f);
	w = sqrt((-2.f * log(w)) / w);
	return complex(x1 * w, x2 * w);
}

/* this RNG doesn't work */

//complex gaussianRandomVariable() {
//	std::default_random_engine gen;
//	std::normal_distribution<float> distribution(0, 1.0f);
//
//	float a = distribution(gen);
//	float b = distribution(gen);
//
//	return complex(a, b);
//}
