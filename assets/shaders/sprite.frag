#version 450

#include "fragdef.h"

void main()
{
    outfragcolor = texture(texturesampler, incoord.xy).xyzw;
}