#version 450

#include "defines/vertdef.h"
#include "defines/defines.h"

void main()
{
    float halfwidth = GetResource(Camera, pushconstants.bindbuffer).viewport.x / 2.0f;
    float halfheight =  GetResource(Camera, pushconstants.bindbuffer).viewport.y / 2.0f;

    gl_Position = vec4(vposition.x / halfwidth - 1.0f, vposition.y / halfheight - 1.0f, 0.0, 1.0);
   
    outcolor = vcolor;
    outcoord = vuv;
}
