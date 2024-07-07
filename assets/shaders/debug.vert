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

layout(set = 0, binding = 0) uniform  CameraBuffer{

    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 viewproj;

	vec4 viewpos;
    vec4 mousepos;
    vec4 viewport;

    vec4 lightcolor;
	vec4 lightpos;
	float ambient;
	float diffuse;
	float specular;

} cameradata;

layout( push_constant ) uniform constants
{
    mat4 rendermatrix;

    int objectID;
    int boneind;
    int debug;
	int debugbones;
} pushconstants;

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
