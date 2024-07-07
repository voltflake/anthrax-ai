#version 450 core

#include "vertdef.h"
// layout (location = 4) in vec4 vweight;
// layout (location = 5) in ivec4 vboneid;

// layout (location = 4) out vec4 outweight;
// layout (location = 5) out ivec4 outboneid;

layout(set = 2, binding = 0) buffer readonly StorageArray
{
    mat4 bonesmatrices[MAX_BONES];
    uint data[512];

} storagedata;


void main()
{
    vec3 localnormal;
    mat4 bonetransforms = storagedata.bonesmatrices[vboneid[0]] * vweight[0];
    bonetransforms += storagedata.bonesmatrices[vboneid[1]] * vweight[1];
    bonetransforms += storagedata.bonesmatrices[vboneid[2]] * vweight[2];
    bonetransforms += storagedata.bonesmatrices[vboneid[3]] * vweight[3];

// mat4 bonetransforms = mat4(0);
//     for(int i = 0 ; i < 4 ; i++)
//     {
//      if(vboneid[i] == -1) {
//         //  bonetransforms = mat4(1);
//         break;
//      }
//         if(vboneid[i] >= MAX_BONES) {
//             bonetransforms = mat4(1);
//             break;
//         }
//     //     //     finpos = vec4(vposition,1.0f);
//     //     //     break;
//     //     }
//         int ind = (vboneid[i]);
//         mat4 localpos = storagedata.bonesmatrices[ind] * vweight[i];;//vec4(vposition,1.0f);
//         bonetransforms += localpos;// * vweight[i];
//        // localnormal = mat3(storagedata.bonesmatrices[ind]) * vnormal;
//     }
		
    vec4 posl = bonetransforms * vec4(vposition,1.0f);
    gl_Position =  pushconstants.rendermatrix * posl;
    outcoord = vuv;

    outweight = vweight;
    outboneid = vboneid;
    outnormal =  mat3(bonetransforms) * vnormal ;

    
//  outnormal = localnormal;


    // gl_Position = pushconstants.rendermatrix * vec4(vposition, 1.0f);
 
    // outnormal = mat3(transpose(inverse( cameradata.model ))) * vnormal;
    // outcoord = vuv;
    // outpos = cameradata.model * vec4(vposition, 1.0);

}