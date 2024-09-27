#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_OutputTexture;

void WritePixelColor(ivec2 coord, vec3 color)
{
    imageStore(u_OutputTexture, coord, vec4(color, 1.0));
}

uniform vec2 u_ScreenSize;
uniform float u_Time;

// Constants
const float PI = 3.14159265359;
const float POSITIVE_INF = 1.0 / 0.0;
const float NEGATIVE_INF = -1.0 / 0.0;

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

struct Ray
{
    vec3 origin;
    vec3 direction;
};

vec3 GetPointOnRay(Ray ray, float t)
{
    return ray.origin + ray.direction * t;
}

struct HitInfo
{
    float t;
    vec3 hitPoint;
    vec3 normal;
};

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

    return info;
}

const uint spheresMaxCount = 100;
Sphere spheres[spheresMaxCount];
uint spheresCount = 0;

vec3 TraceSpheresArray(Ray ray)
{
    vec3 closestSphereColor = vec3(0.0);
    HitInfo closestHit = HitInfo(POSITIVE_INF, vec3(0.0), vec3(0.0));

    for (uint i = 0; i < spheresCount; i++)
    {
        Sphere sphere = spheres[i];
        HitInfo hit = CheckSphereCollision(sphere, ray);

        if (hit.t > -1.0 && hit.t < closestHit.t)
        {
            closestHit = hit;
            closestSphereColor = sphere.material.color;
        }
    }

    return closestSphereColor;
}

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 fragCoord = vec2(texelCoord).xy / u_ScreenSize.xy;
    vec2 uv = fragCoord * 2.0 - 1;

    vec3 color = vec3(0.0, 0.0, 0.0);

    Ray ray = CalcRay(uv);

    spheres[0] = Sphere(vec3(0, 0.15, -4.0), 0.5, Material(vec3(1.0)));
    spheresCount++;
    spheres[1] = Sphere(vec3(0, 0.15, -1), 0.05, Material(vec3(1.0, 0.0, 0.0)));
    spheresCount++;

    color += TraceSpheresArray(ray);

    WritePixelColor(texelCoord, color);
}
