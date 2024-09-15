#version 450

#include "fragdef.h"
#include "defines.h"

void main()
{
    outfragcolor = texture(texturesampler, incoord.xy).xyzw;
}