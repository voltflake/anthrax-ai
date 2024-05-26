#version 450
layout (location = 0) in vec3 vposition;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec3 vcolor;
layout (location = 3) in vec2 vuv;

layout (location = 0) out vec3 outcolor;
layout (location = 1) out vec2 outcoord;

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
 mat4 render_matrix;
} pushconstants;


out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    float halfwidth = cameradata.viewport.x / 2.0f;
    float halfheight = cameradata.viewport.y / 2.0f;

    gl_Position = vec4(vposition.x / halfwidth - 1.0f, vposition.y / halfheight - 1.0f, 0.0, 1.0);
   
    outcolor = vcolor;
    outcoord = vuv;
}
