#ifndef RAY_
#define RAY_

struct Ray
{
    vec3 origin;
    vec3 direction;
};

vec3 GetPointOnRay(Ray ray, float t)
{
    return ray.origin + ray.direction * t;
}

#endif