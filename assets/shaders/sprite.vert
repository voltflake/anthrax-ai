#version 450

#include "vertdef.h"

void main()
{
    float halfwidth = cameradata.viewport.x / 2.0f;
    float halfheight = cameradata.viewport.y / 2.0f;

    gl_Position = vec4(vposition.x / halfwidth - 1.0f, vposition.y / halfheight - 1.0f, 0.0, 1.0);
   
    outcolor = vcolor;
    outcoord = vuv;
}
