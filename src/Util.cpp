#include "Util.h"

#include <random>
#include <ctime>

#include "Application.h"
#include "glew.h"
#include "gl/Shader.h"

/*
Author: S�bastien Rombauts
Link: https://github.com/SRombauts/SimplexNoise/blob/master/src/SimplexNoise.cpp

Modified to use pesudo-random instead of constant table
*/

unsigned int Util::seed_value = static_cast<unsigned int>(time(nullptr));
std::mt19937 Util::rng(seed_value);

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

static float grad(int32_t hash, float x, float y) {
    const int32_t h = hash & 0x3F;  // Convert low 3 bits of hash code
    const float u = h < 4 ? x : y;  // into 8 simple gradient directions,
    const float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v); // and compute the dot product with (x,y).
}

void Util::seed(const unsigned int seed) {
    seed_value = seed;
    rng.seed(seed);
    Util::seed();
}

void Util::seed() {
    srand(seed_value);
    for (uint8_t& i : perm) {
        i = static_cast<uint8_t>(rand() % 256);
    }
}

// range [-1,1]
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

// range [-1,1]
float Util::noise(float x, float y) {
    float n0, n1, n2;   // Noise contributions from the three corners

    // Skewing/Unskewing factors for 2D
    static const float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
    static const float G2 = 0.211324865f;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

    // Skew the input space to determine which simplex cell we're in
    const float s = (x + y) * F2;  // Hairy factor for 2D
    const float xs = x + s;
    const float ys = y + s;
    const int32_t i = fastfloor(xs);
    const int32_t j = fastfloor(ys);

    // Unskew the cell origin back to (x,y) space
    const float t = static_cast<float>(i + j) * G2;
    const float X0 = i - t;
    const float Y0 = j - t;
    const float x0 = x - X0;  // The x,y distances from the cell origin
    const float y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
        i1 = 1;
        j1 = 0;
    } else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
        i1 = 0;
        j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    const float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
    const float y1 = y0 - j1 + G2;
    const float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
    const float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    const int gi0 = hash(i + hash(j));
    const int gi1 = hash(i + i1 + hash(j + j1));
    const int gi2 = hash(i + 1 + hash(j + 1));

    // Calculate the contribution from the first corner
    float t0 = 0.5f - x0*x0 - y0*y0;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0);
    }

    // Calculate the contribution from the second corner
    float t1 = 0.5f - x1*x1 - y1*y1;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1);
    }

    // Calculate the contribution from the third corner
    float t2 = 0.5f - x2*x2 - y2*y2;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 45.23065f * (n0 + n1 + n2);
}

glm::vec2 Util::rotate(const glm::vec2& point, const float angle)
{
    const float cosTheta = cosf(angle);
    const float sinTheta = sinf(angle);

    return glm::vec2(
        point.x * cosTheta - point.y * sinTheta,
        point.x * sinTheta + point.y * cosTheta
    );
}

void Util::decreaseMagnitude(glm::vec2& vector, const float scaler)
{
    decreaseMagnitude(vector.x,scaler);
    decreaseMagnitude(vector.y,scaler);
}

void Util::decreaseMagnitude(float& vector, const float scaler)
{
    if (vector < 0)
    {
        vector += scaler;
        if (vector > 0)
            vector = 0;
    } else
    {
        vector -= scaler;
        if (vector < 0)
            vector = 0;
    }
}

void Util::drawDebugLines(const glm::vec2* points, int size)
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < size; ++i) {
        glVertex2f((points[i].x - world::origin.x) * world::PIXEL_SCALE, (points[i].y - world::origin.y) * world::PIXEL_SCALE);
    }
    glEnd();
}
