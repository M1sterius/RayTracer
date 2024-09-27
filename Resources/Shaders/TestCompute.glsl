#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_OutputTexture;

void WritePixelColor(ivec2 coord, vec3 color)
{
    imageStore(u_OutputTexture, coord, vec4(color, 1.0));
}

struct Ray
{
    vec3 origin;
    vec3 direction;
};

vec3 GetPointOnRay(Ray ray, float t)
{
    return ray.origin + ray.direction * t;
}

uniform vec2 u_ScreenSize;
uniform float u_Time;

// Constants
const float PI = 3.14159265359;

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

float CheckSphereCollision(vec3 center, float radius, Ray ray)
{
    vec3 oc = center - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float b = -2.0 * dot(ray.direction, oc);
    float c = dot(oc, oc) - radius * radius;
    float disc = b * b - 4 * a * c;

    if (disc < 0) return -1.0;
    else return (-b - sqrt(disc)) / (2 * a);
}

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 fragCoord = vec2(texelCoord).xy / u_ScreenSize.xy;
    vec2 uv = fragCoord * 2.0 - 1;

    vec3 color = vec3(0, 0, 0.0);

    Ray ray = CalcRay(uv);

    vec3 spherePos = vec3(0, 0.15, -1);
    float t = CheckSphereCollision(spherePos, 0.1, ray);
    if (t > -1.0)
    {
        vec3 pnt = GetPointOnRay(ray, t);
        vec3 n = normalize(pnt - spherePos);
        color += n * 0.5 + 0.5;
    }

    WritePixelColor(texelCoord, color);
}
