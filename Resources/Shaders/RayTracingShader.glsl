#version 460 core

#ifndef RAY_TRACING_SHADER_
#define RAY_TRACING_SHADER_

#include "Ray.glsl"
#include "Random.glsl"
#include "Constants.glsl"

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_OutputTexture;

void WritePixelColor(ivec2 coord, vec3 color)
{
    imageStore(u_OutputTexture, coord, vec4(color, 1.0));
}

uniform vec2 u_ScreenSize;
uniform float u_Time;
uniform uint u_MaxReflectionsCount;
uniform uint u_RaysPerPixel;

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

// vec4s are used for alignment reasons only
struct Material
{
    vec4 color;
    vec4 emission; // xyz - emission color, w - emission strength
    float smoothness;
};

struct Sphere
{
    vec3 center;
    float radius;
    Material material;
};

struct HitInfo
{
    float t;
    vec3 hitPoint;
    vec3 normal;
    Material material;
};

layout(std430, binding = 1) buffer s_SpheresBuffer
{
    Sphere SSBO_spheres[];
};
uniform uint u_SSBOSpheresCount;

HitInfo CheckSphereCollision(Sphere sphere, Ray ray)
{
    HitInfo info;

    const vec3 oc = sphere.center - ray.origin;
    const float a = dot(ray.direction, ray.direction); // a way to calculate squared length
    const float h = dot(ray.direction, oc);
    const float c = dot(oc, oc) - sphere.radius * sphere.radius;
    const float disc = h * h - a * c;

    info.t = disc < 0 ? -1.0 : ((h - sqrt(disc)) / a);
    info.hitPoint = GetPointOnRay(ray, info.t);
    info.normal = normalize(info.hitPoint - sphere.center);
    info.material = sphere.material;

    return info;
}

HitInfo CalculateRaySpheresCollision(Ray ray)
{
    HitInfo closestHit = HitInfo(POSITIVE_INF, vec3(0.0), vec3(0.0), Material(vec4(0.0), vec4(0.0), 0.0));

    for (uint i = 0; i < u_SSBOSpheresCount; i++)
    {
        Sphere sphere = SSBO_spheres[i];
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

    for (uint i = 0; i < u_MaxReflectionsCount; i++)
    {
        HitInfo hitInfo = CalculateRaySpheresCollision(ray);
        if (hitInfo.t > -1.0)
        {
            ray.origin = hitInfo.hitPoint;
            ray.direction = -RandomHemisphereDirection(hitInfo.normal, rngState); // TODO: Figure out why it needs the minus to work

            Material material = hitInfo.material;
            vec3 emittedLight = vec3(material.emission.xyz * material.emission.w);
            light += emittedLight * rayColor;
            rayColor *= vec3(material.color);
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

    Ray ray = CalcRay(uv);

    vec3 color = vec3(0.0);

    for (uint i = 0; i < u_RaysPerPixel; i++)
    {
        color += Trace(ray, rngState);
    }

    color /= u_RaysPerPixel;

    WritePixelColor(texelCoord, color);
}

#endif
