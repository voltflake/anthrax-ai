layout (location = 0) in vec3 vposition;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec3 vcolor;
layout (location = 3) in vec2 vuv;

layout (location = 0) out vec4 outpos;
layout (location = 1) out vec3 outnormal;
layout (location = 2) out vec3 outcolor;
layout (location = 3) out vec2 outcoord;

layout(set = 0, binding = 0) uniform  CameraBuffer {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 viewproj;
	vec4 viewpos;
    vec4 pos;
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
} pushconstants;

out gl_PerVertex {
    vec4 gl_Position;
};