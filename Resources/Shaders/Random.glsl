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

float RandomValueNormalDistribution(inout uint state)
{
    float theta = 2 * PI * RandomFloat(state);
    float rho = sqrt(-2.0 * log(RandomFloat(state)));
    return rho * cos(theta);
}

vec3 RandomDirection(inout uint state)
{
    float x = RandomValueNormalDistribution(state);
    float y = RandomValueNormalDistribution(state);
    float z = RandomValueNormalDistribution(state);
    return normalize(vec3(x, y, z));
}

vec3 RandomHemisphereDirection(vec3 normal, inout uint state)
{
    vec3 dir = RandomDirection(state);
    return dir * sign(dot(normal, dir));
}

#endif