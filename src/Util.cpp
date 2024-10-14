#include "Util.h"

#include <random>
#include <ctime>

/*
Author: S�bastien Rombauts
Link: https://github.com/SRombauts/SimplexNoise/blob/master/src/SimplexNoise.cpp

Modified to use pesudo-random instead of constant table
*/

uint8_t perm[256];

inline int32_t fastfloor(const float fp) {
    int32_t i = static_cast<int32_t>(fp);
    return (fp < i) ? (i - 1) : (i);
}

inline uint8_t hash(const int32_t i) {
    return perm[static_cast<uint8_t>(i)];
}

float grad(int32_t hash, float x) {
    const int32_t h = hash & 0x0F;  // Convert low 4 bits of hash code
    float grad = 1.0f + (h & 7);    // Gradient value 1.0, 2.0, ..., 8.0
    if ((h & 8) != 0) grad = -grad; // Set a random sign for the gradient
    //  float grad = gradients1D[h];    // NOTE : Test of Gradient look-up table instead of the above
    return (grad * x);              // Multiply the gradient with the distance
}


void Util::seed(const unsigned int seed) {
    srand(seed);
    for (auto i = 0; i < 256; i++) {
        perm[i] = static_cast<uint8_t>(rand() % 256);
    }
}

void Util::seed() {
    Util::seed(static_cast<unsigned int>(time(nullptr)));
}


float Util::noise(float x) {
    float n0, n1;   // Noise contributions from the two "corners"

    // No need to skew the input space in 1D

    // Corners coordinates (nearest integer values):
    int32_t i0 = fastfloor(x);
    int32_t i1 = i0 + 1;
    // Distances to corners (between 0 and 1):
    float x0 = x - i0;
    float x1 = x0 - 1.0f;

    // Calculate the contribution from the first corner
    float t0 = 1.0f - x0 * x0;
    //  if(t0 < 0.0f) t0 = 0.0f; // not possible
    t0 *= t0;
    n0 = t0 * t0 * grad(hash(i0), x0);

    // Calculate the contribution from the second corner
    float t1 = 1.0f - x1 * x1;
    //  if(t1 < 0.0f) t1 = 0.0f; // not possible
    t1 *= t1;
    n1 = t1 * t1 * grad(hash(i1), x1);

    // The maximum value of this noise is 8*(3/4)^4 = 2.53125
    // A factor of 0.395 scales to fit exactly within [-1,1]
    return 0.395f * (n0 + n1);
}