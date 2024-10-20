#version 450

#include "defines/vertdef.h"
#include "defines/defines.h"

void main()
{
    gl_Position = pushconstants.rendermatrix * vec4(vposition, 1.0f);
 
    outnormal = vnormal;
    outcoord = vuv;
    outpos = vec4(vposition, 1.0f);//GetResource(Camera, pushconstants.bindbuffer).model * vec4(vposition, 1.0);
}
