#version 450

#include "defines/vertdef.h"

vec3 plane[6] = vec3[](
    vec3(-1, 1, 0), vec3(-1, -1, 0),vec3(1, -1, 0), 
    vec3(1, -1, 0),  vec3(-1, -1, 0),vec3(1, -1, 0)
);

void main()
{
    gl_Position = vec4(plane[gl_VertexIndex].xyz, 1.0);

    outcolor = vcolor;
    outcoord = vuv;
}
