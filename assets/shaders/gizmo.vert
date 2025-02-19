#version 460

#include "defines/vertdef.h"
#include "defines/defines.h"
//#extension GL_EXT_debug_printf : enable

void main()
{
    mat4 rendermatrix = GetResource(Camera, GetUniformInd()).proj * GetResource(Camera, GetUniformInd()).view * (GetResource(Instance, GetInstanceInd()).instances[gl_BaseInstance].rendermatrix );
    
    vec4 position = vec4(vposition.xyz, 1.0f) ;//* vec4(0.08,0.08,0.08,1.0);

    gl_Position = rendermatrix * position ;
    

    //debugPrintfEXT("instanceIndex=%d\n", gl_BaseInstance);
    outnormal = vnormal;
    outcoord = vuv;
    outweight = vweight;
    outboneid = vboneid;
    outpos = vec4(vposition.xyz, 1.0f);
}
