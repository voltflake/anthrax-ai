#version 450

#include "helpers.h"

layout (location = 0) in vec3 vposition;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec3 vcolor;
layout (location = 3) in vec2 vuv;
// layout (location = 4) in vec4 vboneid;
// layout (location = 5) in vec4 vweight;

layout (location = 0) out vec3 outnear;
layout (location = 1) out vec3 outfar;

#include "defines.h"

out gl_PerVertex {
    vec4 gl_Position;
};

vec3 gridplane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

void main() {
    vec3 pos = gridplane[gl_VertexIndex].xyz;

    outnear = UnprojectPos(vec3(pos.x, pos.y, 0.0), cameradata.view, cameradata.proj);
    outfar = UnprojectPos(vec3(pos.x, pos.y, 1.0), cameradata.view, cameradata.proj);
    gl_Position = vec4(pos, 1.0); 
}
