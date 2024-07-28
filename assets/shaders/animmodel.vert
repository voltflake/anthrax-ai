#version 450 core

#include "vertdef.h"

layout(set = 2, binding = 0) buffer readonly StorageArray
{
    uint data[512];

} storagedata;


layout(set = 3, binding = 0) uniform  TransformsBuffer
{
    mat4 bonesmatrices[MAX_BONES];

} transforms;

void main()
{
    vec3 localnormal;
    mat4 bonetransforms = transforms.bonesmatrices[vboneid[0]] * vweight[0];
    bonetransforms += transforms.bonesmatrices[vboneid[1]] * vweight[1];
    bonetransforms += transforms.bonesmatrices[vboneid[2]] * vweight[2];
    bonetransforms += transforms.bonesmatrices[vboneid[3]] * vweight[3];

    vec4 posl = bonetransforms * vec4(vposition,1.0f);
    gl_Position =  pushconstants.rendermatrix * posl;
    outcoord = vuv;

    outweight = vweight;
    outboneid = vboneid;
    outnormal =  mat3(bonetransforms) * vnormal ;

}