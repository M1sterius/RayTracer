#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_OutputTexture;

uniform vec2 u_ScreenSize;
uniform float u_Time;

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 fragCoord = vec2(texelCoord).xy / u_ScreenSize.xy;
    vec2 uv = fragCoord * 2.0 - 1;

//    uv *= vec2(u_ScreenSize.x / u_ScreenSize.y, 1);

    vec3 color = vec3(0.0, 0.0, 0.0);

    float r = 0.17;
    for (float i = 0.0; i < 60.0; i++)
    {
        float a = i / 3;
        float dx = 2 * r * cos(a) - r * cos(2 * a);
        float dy = 2 * r * sin(a) - r * sin(2 * a);

        color += 0.001 / length(uv - vec2(dx + 0.1, dy));
    }

    imageStore(u_OutputTexture, texelCoord, vec4(color, 1.0));
}
