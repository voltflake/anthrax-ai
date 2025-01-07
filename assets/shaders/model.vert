#version 460

#include "defines/vertdef.h"
#include "defines/defines.h"
//#extension GL_EXT_debug_printf : enable

void main()
{
    uint hasanim = GetResource(Instance, GetInstanceInd()).instances[gl_BaseInstance].hasanimation;
    mat4 bonetransforms = mat4(1.0f);

    if (hasanim == 1) {
        bonetransforms = GetResource(Instance, GetInstanceInd()).instances[gl_BaseInstance].bonesmatrices[vboneid[0]] * vweight[0];
        bonetransforms += GetResource(Instance, GetInstanceInd()).instances[gl_BaseInstance].bonesmatrices[vboneid[1]] * vweight[1];
        bonetransforms += GetResource(Instance, GetInstanceInd()).instances[gl_BaseInstance].bonesmatrices[vboneid[2]] * vweight[2];
        bonetransforms += GetResource(Instance, GetInstanceInd()).instances[gl_BaseInstance].bonesmatrices[vboneid[3]] * vweight[3];
    }

    mat4 rendermatrix = GetResource(Camera, GetUniformInd()).proj * GetResource(Camera, GetUniformInd()).view * GetResource(Instance, GetInstanceInd()).instances[gl_BaseInstance].rendermatrix;
    
    vec4 position = bonetransforms * vec4(vposition.xyz, 1.0f);
    gl_Position = rendermatrix * position;

    //debugPrintfEXT("instanceIndex=%d\n", gl_BaseInstance);
    outnormal = vnormal;
    outcoord = vuv;
    outweight = vweight;
    outboneid = vboneid;
    outpos = vec4(vposition.xyz, 1.0f);//GetResource(Camera, pushconstants.bindbuffer).model * vec4(vposition, 1.0);
}
