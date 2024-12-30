#version 460 core

#ifndef RAY_TRACING_SHADER_
#define RAY_TRACING_SHADER_

#include "Ray.glsl"
#include "Random.glsl"
#include "Constants.glsl"
#include "Material.glsl"

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_OutputTexture;

uniform vec2 u_ScreenSize;
uniform uint u_MaxReflectionsCount;
uniform uint u_RaysPerPixel;
uniform uint u_FrameIndex;
uniform uint u_RandomSeed;

// Camera properties uniforms
uniform vec3 u_CameraPosition;
uniform vec3 u_CameraForward;
uniform vec3 u_CameraUp;
uniform vec3 u_CameraRight;
uniform vec2 u_HalfViewportSize;

void WritePixelColor(ivec2 coord, vec3 color)
{
    vec3 prevColor = imageLoad(u_OutputTexture, coord).xyz;

    float blend = 0.01;
    vec3 col = prevColor * (1 - blend) + color * blend;

    imageStore(u_OutputTexture, coord, vec4(col, 1.0));
}

Ray CalcRay(vec2 uv)
{
    Ray ray;
    ray.origin = u_CameraPosition;
    const vec2 screenSpaceViewport = u_HalfViewportSize * uv;

    ray.direction = normalize(u_CameraRight * screenSpaceViewport.x + u_CameraUp *
    screenSpaceViewport.y + u_CameraForward);

    return ray;
}

struct Triangle
{
    // vec4 used to comply with std430 alignment , w component does nothing
    vec4 v0;
    vec4 v1;
    vec4 v2;
};

struct Mesh
{
    uint TrianglesStartIndex; // Index to the first triangle of the mesh in the triangles buffer
    uint TrianglesCount; // The amount of triangles this mesh consists of in the triangles buffer
    Material material;
};

struct HitInfo
{
    float t;
    vec3 hitPoint;
    vec3 normal;
    uint HitMeshID;
};

layout(std430, binding = 1) buffer ssbo_MeshBuffer
{
    Mesh ssbo_Meshes[MESHES_COUNT_LIMIT];
    Triangle ssbo_Triangles[];
};
uniform uint u_MeshesCount;

HitInfo RayTriangleCollision(Ray ray, Triangle triangle)
{
    const float epsilon = 0.000001;
    HitInfo hit = HitInfo(-1.0, vec3(0.0), vec3(0.0), 0);

    vec3 edge1 = (triangle.v1 - triangle.v0).xyz;
    vec3 edge2 = (triangle.v2 - triangle.v0).xyz;
    vec3 ray_cross_e2 = cross(ray.direction, edge2);
    float det = dot(edge1, ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return hit;

    float inv_det = 1.0f / det;
    vec3 s = ray.origin - triangle.v0.xyz;
    float u = inv_det * dot(s, ray_cross_e2);

    if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u-1) > epsilon))
        return hit;

    vec3 s_cross_e1 = cross(s, edge1);
    float v = inv_det * dot(ray.direction, s_cross_e1);

    if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon))
        return hit;

    float t = inv_det * dot(edge2, s_cross_e1);

    if (t > epsilon) {
        hit.t = t;
        hit.hitPoint = ray_origin + ray_vector * t;
        hit.normal = normalize(cross(edge1, edge2));
        return hit;
    }

    return hit;
}

HitInfo CalculateRayCollision(Ray ray)
{
    HitInfo closestHit = HitInfo(POSITIVE_INF, vec3(0.0), vec3(0.0), 0);

    // Go through all the meshes
    for (uint i = 0; i < u_MeshesCount; i++)
    {
        Mesh mesh = ssbo_Meshes[i];
        uint startIndex = mesh.TrianglesStartIndex;

        // Go through all triangles current mesh consists of
        for (uint j = startIndex; j < startIndex + mesh.TrianglesCount; i++)
        {
            Triangle triangle = ssbo_Triangles[j];
            HitInfo hit = RayTriangleCollision(ray, triangle);

            if (hit.t < closestHit.t)
            {
                closestHit = hit;
                closestHit.HitMeshID = i;
            }
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
        HitInfo hitInfo = CalculateRayCollision(ray);

        if (hitInfo.t == POSITIVE_INF) return light;

        if (hitInfo.t > -1.0)
        {
            ray.origin = hitInfo.hitPoint;
            ray.direction = normalize(hitInfo.normal + RandomDirection(rngState));

            Material material = ssbo_Meshes[hitInfo.HitMeshID].material;
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

    Ray ray = CalcRay(uv);

    vec3 color = vec3(0.0);

    for (uint i = 0; i < u_RaysPerPixel; i++)
    {
        color += Trace(ray, rngState);
    }

    color /= u_RaysPerPixel;
    color = sqrt(color / float(u_RaysPerPixel)); // gamma correction

    WritePixelColor(texelCoord, color);
}

#endif
