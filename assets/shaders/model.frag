#version 450

#include "defines/fragdef.h"
#include "defines/defines.h"


void main()
{
    vec4 mousepos = GetResource(Camera, GetUniformInd()).mousepos;

    uint depth_ind = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);
    if( length(mousepos.xy - gl_FragCoord.xy) < 2)
    {
        GetResource(Storage, GetStorageInd()).data[depth_ind] = pushconstants.objectID;
    }

    vec4 color = texture(textures[GetTextureInd()], incoord.xy).xyzw;
    if (pushconstants.selected == 1) {
        color.rgb += vec3(0.1);
        color.rgb = clamp(color.rgb, 0.0, 1.0);
    }
    if (pushconstants.boneID != -1) {
        for (int i = 0; i < 4; i++) {
            if (pushconstants.boneID == inboneid[i]) {
                if (inweight[i] >= 0.7) {
                    color.xyz = vec3(1, 0, 0) * inweight[i];
                }
                else if (inweight[i] >= 0.4 && inweight[i] <= 0.6) {
                    color.xyz = vec3(0, 1, 0) * inweight[i];
                }
                else if (inweight[i] >= 0.1) {
                    color.xyz = vec3(0, 0, 1) * inweight[i];
                }
            }
        }
    }
    outfragcolor = color;
}
