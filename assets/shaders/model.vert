#version 450

#include "vertdef.h"

void main()
{
    gl_Position = pushconstants.rendermatrix * vec4(vposition, 1.0f);
 
    outnormal = mat3(transpose(inverse( cameradata.model ))) * vnormal;
    outcoord = vuv;
    outpos = cameradata.model * vec4(vposition, 1.0);
}
