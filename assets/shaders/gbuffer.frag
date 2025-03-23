#version 450

#include "defines/defines.h"
#include "defines/fragdef.h"
#
layout (location = 1) out vec4 outnormal;
layout (location = 2) out vec4 outposition;
void main()
{
    vec2 uv = incoord.xy;
    uv.y *= -1;
    vec4 color = texture(textures[GetTextureInd()], uv.xy).xyzw;
    outfragcolor = vec4(color);
    outnormal = vec4(innormal, 1) ;
    outposition = vec4(inpos) ;
}

