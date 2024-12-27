#version 460 core

#ifndef RAY_TRACING_SHADER_
#define RAY_TRACING_SHADER_

#include "Ray.glsl"
#include "Random.glsl"
#include "Constants.glsl"

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_OutputTexture;

uniform vec2 u_ScreenSize;
uniform uint u_MaxReflectionsCount;
uniform uint u_RaysPerPixel;
uniform uint u_FrameIndex;
uniform uint u_RandomSeed;

void WritePixelColor(ivec2 coord, vec3 color)
{
//    vec3 prevColor = imageLoad(u_OutputTexture, coord).xyz;
//
//    float blend = 0.01;
//    vec3 col = prevColor * (1 - blend) + color * blend;

//    vec3 col = prevColor + color;

    imageStore(u_OutputTexture, coord, vec4(color, 1.0));
}

// Camera properties uniforms
uniform vec3 u_CameraPosition;
uniform vec3 u_CameraForward;
uniform vec3 u_CameraUp;
uniform vec3 u_CameraRight;
uniform vec2 u_HalfViewportSize;

Ray CalcRay(vec2 uv)
{
    Ray ray;
    ray.origin = u_CameraPosition;
    const vec2 screenSpaceViewport = u_HalfViewportSize * uv;

    ray.direction = normalize(u_CameraRight * screenSpaceViewport.x + u_CameraUp *
    screenSpaceViewport.y + u_CameraForward);

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
    info.t = -1.0;

    vec3 oc = sphere.center - ray.origin;
    float a = 1; // because rays are always normalized
    float h = dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float disc = h * h - a * c;

    if (disc < 0) return info;

    info.t = ((h - sqrt(disc)) / a);
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

        if (hitInfo.t == POSITIVE_INF) return light;

        if (hitInfo.t > -1.0)
        {
            ray.origin = hitInfo.hitPoint;
            ray.direction = normalize(hitInfo.normal + RandomDirection(rngState));

            Material material = hitInfo.material;
            vec3 emittedLight = vec3(material.emission.xyz) * (material.emission.w * float(u_RaysPerPixel));
            light += emittedLight * rayColor;
            rayColor *= material.color.xyz;
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
    uint rngState = GenerateRNGState(texelCoord, u_FrameIndex, u_RandomSeed);
//    uint rngState = (texelCoord.x * texelCoord.y) + u_FrameIndex * 67829345;

    Ray ray = CalcRay(uv);

    vec3 color = vec3(0.0);

    for (uint i = 0; i < u_RaysPerPixel; i++)
    {
        color += Trace(ray, rngState);
    }

    color /= u_RaysPerPixel;

    color = sqrt(color / float(u_RaysPerPixel));

    WritePixelColor(texelCoord, color);
}

#endif
