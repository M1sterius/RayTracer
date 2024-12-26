#ifndef RANDOM_
#define RANDOM_

#include "Constants.glsl"

float RandomFloat(inout uint state)
{
    state = state * 747796405 + 2891336453;
    uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
    result = (result >> 22) ^ result;
    return result / 4294967295.0;
}

float RandomFloatNormalDistribution(inout uint state)
{
    float theta = 2 * PI * RandomFloat(state);
    float rho = sqrt(-2.0 * log(max(RandomFloat(state), 1e-6)));
    return rho * cos(theta);
}

vec3 RandomDirection(inout uint state)
{
    float x = RandomFloatNormalDistribution(state);
    float y = RandomFloatNormalDistribution(state);
    float z = RandomFloatNormalDistribution(state);
    return normalize(vec3(x, y, z));
//
//    for (uint i = 0; i < 100; i++)
//    {
//        float x = RandomFloat(state) * 2 - 1;
//        float y = RandomFloat(state) * 2 - 1;
//        float z = RandomFloat(state) * 2 - 1;
//        vec3 point = vec3(x, y, z);
//        float dSqr = dot(point, point);
//        if (dSqr <= 1) return point;
//    }
//
//    return vec3(0.0);
}

vec3 RandomHemisphereDirection(vec3 normal, inout uint state)
{
    vec3 dir = RandomDirection(state);
    if (dot(normal, dir) >= 0) return dir;
    else return -dir;
}
uint Hash(uint x)
{
    x = (x ^ 61u) ^ (x >> 16u);
    x *= 9u;
    x ^= x >> 4u;
    x *= 0x27d4eb2du;
    x ^= x >> 15u;
    return x;
}

uint GenerateRNGState(ivec2 texelCoord, uint u_FrameIndex, uint u_RandomSeed)
{
    // Combine texelCoord.x and texelCoord.y into a single value
    uint coordHash = Hash(uint(texelCoord.x) ^ (uint(texelCoord.y) * 0x27d4eb2du));

    // Hash the combined coordinates with frame index and random seed
    uint rngState = Hash(coordHash ^ Hash(u_FrameIndex) ^ Hash(u_RandomSeed));
    return rngState;
}

#endif