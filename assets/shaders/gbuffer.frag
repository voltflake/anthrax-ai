#version 450

#include "defines/defines.h"
#include "defines/fragdef.h"

layout (location = 1) out vec4 outnormal;
layout (location = 2) out vec4 outposition;
void main()
{
    vec2 uv = incoord.xy;
    uv.y *= -1;
    vec4 color = texture(textures[GetTextureInd()], uv.xy).xyzw;

    vec4 mousepos = GetResource(Camera, GetUniformInd()).mousepos;
    uint depth_ind = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);
    if( length(mousepos.xy - gl_FragCoord.xy) < 1)
    {
        GetResource(Storage, GetStorageInd()).data[depth_ind] = pushconstants.objectID;
    }

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

    outfragcolor = vec4(color);
    outnormal = vec4(innormal, 1);
    outposition = vec4(inpos);

}
