#ifndef MATERIAL_
#define MATERIAL_

// vec4s are used for alignment reasons only
struct Material
{
    vec4 color;
    vec4 emission; // xyz - emission color, w - emission strength
    float smoothness;
};

#endif