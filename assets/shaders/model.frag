#version 450

#include "defines/fragdef.h"
#include "defines/defines.h"

RegisterBuffer(std430, writeonly, Storage, {
    mat4 bonesmatrices[MAX_BONES];
    uint data[DEPTH_ARRAY_SCALE]; 
});

void main()
{
    vec4 mousepos = GetResource(Camera, pushconstants.bindbuffer).mousepos;

    uint depth_ind = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);
    if( length(mousepos.xy - gl_FragCoord.xy) < 1)
    {
        GetResource(Storage, pushconstants.bindstorage).data[depth_ind] = pushconstants.objectID;
    }

    
    vec4 color = texture(textures[pushconstants.bindtexture], incoord.xy).xyzw;
    if (pushconstants.selected == 1) {
        color.r += 0.5;
        clamp(color.r, 0.0, 1.0);
    }
    outfragcolor = color;
}
