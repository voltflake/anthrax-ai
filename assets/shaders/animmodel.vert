#version 450 core

#include "vertdef.h"

layout(set = 3, binding = 0) buffer readonly StorageArray
{
    mat4 bonesmatrices[MAX_BONES];
    uint data[DEPTH_ARRAY_SCALE];
} storagedata;


void main()
{
    vec3 localnormal;
    mat4 bonetransforms = storagedata.bonesmatrices[vboneid[0]] * vweight[0];
    bonetransforms += storagedata.bonesmatrices[vboneid[1]] * vweight[1];
    bonetransforms += storagedata.bonesmatrices[vboneid[2]] * vweight[2];
    bonetransforms += storagedata.bonesmatrices[vboneid[3]] * vweight[3];

    vec4 posl = bonetransforms * vec4(vposition,1.0f);
    gl_Position =  pushconstants.rendermatrix * posl;
    outcoord = vuv;

    outweight = vweight;
    outboneid = vboneid;
    outnormal =  mat3(bonetransforms) * vnormal ;

}