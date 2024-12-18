#version 460 core

#ifndef RAY_TRACING_SHADER_
#define RAY_TRACING_SHADER_

#include "Ray.glsl"
#include "Random.glsl"
#include "Constants.glsl"

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_OutputTexture;

void WritePixelColor(ivec2 coord, vec3 color)
{
    imageStore(u_OutputTexture, coord, vec4(color, 1.0));
}

uniform vec2 u_ScreenSize;
uniform float u_Time;

// Screen
float aspect = u_ScreenSize.x / u_ScreenSize.y;

// Camera
const vec3 cameraPos = vec3(0, 0, 0);
const float FOVY = PI / 4;
const float focalLength = 1.0;
const vec3 cameraForward = vec3(0.0, 0.0, -1.0);
const vec3 cameraUp = vec3(0.0, 1.0, 0.0);
const vec3 cameraRight = vec3(1.0, 0, 0.0);

// Viewport
const float tanFOVY = tan(FOVY / 2);
vec2 halfViewportSize = vec2(tanFOVY * aspect, tanFOVY) * focalLength;

Ray CalcRay(vec2 uv)
{
    Ray ray;
    ray.origin = cameraPos;
    const vec2 screenSpaceViewport = halfViewportSize * uv;

    ray.direction = normalize(cameraRight * screenSpaceViewport.x + cameraUp *
    screenSpaceViewport.y + cameraForward);

    return ray;
}

struct Material
{
    vec3 color;
    vec3 emissionColor;
    float emissionStrength;
};

struct HitInfo
{
    float t;
    vec3 hitPoint;
    vec3 normal;
    Material material;
};

struct Sphere
{
    vec3 center;
    float radius;
    Material material;
};

HitInfo CheckSphereCollision(Sphere sphere, Ray ray)
{
    HitInfo info;

    vec3 oc = sphere.center - ray.origin;
    float a = dot(ray.direction, ray.direction); // a way to calculate squared length
    float h = dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float disc = h * h - a * c;

    info.t = disc < 0 ? -1.0 : ((h - sqrt(disc)) / a);
    info.hitPoint = GetPointOnRay(ray, info.t);
    info.normal = normalize(info.hitPoint - sphere.center);
    info.material = sphere.material;

    return info;
}

const uint spheresMaxCount = 100;
Sphere spheres[spheresMaxCount];
uint spheresCount = 0;

HitInfo CalculateRaySpheresCollision(Ray ray)
{
    HitInfo closestHit = HitInfo(POSITIVE_INF, vec3(0.0), vec3(0.0), Material(vec3(0.0), vec3(0.0), 0.0));

    for (uint i = 0; i < spheresCount; i++)
    {
        Sphere sphere = spheres[i];
        HitInfo hit = CheckSphereCollision(sphere, ray);

        bool didHit = hit.t > -1.0;

        if (didHit && hit.t < closestHit.t)
        {
            closestHit = hit;
        }
    }

    return closestHit;
}

vec3 Trace(Ray ray, inout uint rngState)
{
    vec3 light = vec3(0.0);
    vec3 rayColor = vec3(1.0);

    for (uint i = 0; i < 16; i++)
    {
        HitInfo hitInfo = CalculateRaySpheresCollision(ray);
        if (hitInfo.t > -1.0)
        {
            ray.origin = hitInfo.hitPoint;
            ray.direction = -RandomHemisphereDirection(hitInfo.normal, rngState); // TODO: Figure out why it needs the minus to work

            Material material = hitInfo.material;
            vec3 emittedLight = material.emissionColor * material.emissionStrength;
            light += emittedLight * rayColor;
            rayColor *= material.color;
        }
        else
        {
            break;
        }
    }

    return light;
}

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 fragCoord = vec2(texelCoord).xy / u_ScreenSize.xy;
    vec2 uv = fragCoord * 2.0 - 1;
    uint rngState = texelCoord.x * texelCoord.y;

    spheres[0] = Sphere(vec3(-0.5, 0.4, -2.0), 0.3, Material(vec3(1.0, 0.0, 0.0), vec3(0.0), 0.0));
    spheresCount++;
    spheres[1] = Sphere(vec3(0.2, 0, -2.0), 0.1, Material(vec3(0.4, 0.3, 0.2), vec3(0.0), 0.0));
    spheresCount++;
    spheres[2] = Sphere(vec3(0.0, -0.15, -2.0), 0.1, Material(vec3(0.3, 0.5, 0.1), vec3(1.0), 1.0));
    spheresCount++;
    spheres[3] = Sphere(vec3(0.3, -0.6, -2.3), 0.5, Material(vec3(1.0), vec3(0.0), 0.0));
    spheresCount++;

    Ray ray = CalcRay(uv);
    vec3 color = Trace(ray, rngState);

    WritePixelColor(texelCoord, color);
}

#endif
