#version 450

#include "defines/vertdef.h"
#include "defines/defines.h"

void main()
{
    float halfwidth = GetResource(Camera, GetUniformInd()).viewport.x / 2.0f;
    float halfheight =  GetResource(Camera, GetUniformInd()).viewport.y / 2.0f;

    gl_Position = vec4(vposition.x / halfwidth - 1.0f, vposition.y / halfheight - 1.0f, 0.0, 1.0);

    outcolor = vcolor;
    outcoord = vuv;
    outpos = gl_Position;
}
