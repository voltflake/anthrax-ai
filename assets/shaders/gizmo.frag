#version 450

#include "defines/fragdef.h"
#include "defines/defines.h"


void main()
{
    vec4 mousepos = GetResource(Camera, GetUniformInd()).mousepos;

    uint depth_ind = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);
    if( length(mousepos.xy - gl_FragCoord.xy) < 3)
    {
        GetResource(Storage, GetStorageInd()).data[depth_ind] = pushconstants.objectID;
    }

    vec4 color = texture(textures[GetTextureInd()], incoord.xy).xyzw;
    if (pushconstants.selected == 1) {
        color.rgb += vec3(0.1);
        color.rgb = clamp(color.rgb, 0.0, 1.0);
    }

    int gizmo = pushconstants.gizmo;
    if (gizmo == GIZMO_X)
    {
        color.rgb = vec3(0,1,0);
    }
    if (gizmo == GIZMO_Y){
            color.rgb = vec3(0,0,1);
    }
    if (gizmo == GIZMO_Z) {
            color.rgb = vec3(1,0,0);
    }

    outfragcolor = color;
}
