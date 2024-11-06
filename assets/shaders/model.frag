#version 450

#include "defines/fragdef.h"
#include "defines/defines.h"

void main()
{
    outfragcolor = texture(textures[pushconstants.bindtexture], incoord.xy).xyzw;
}
