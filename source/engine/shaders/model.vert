#version 450
layout (location = 0) in vec3 vposition;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec3 vcolor;
layout (location = 3) in vec2 vuv;

layout (location = 0) out vec4 outpos;
layout (location = 1) out vec3 outnormal;
layout (location = 2) out vec2 outcoord;

layout(set = 0, binding = 0) uniform  CameraBuffer{
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

} cameraData;

layout( push_constant ) uniform constants
{
    mat4 rendermatrix;
} pushconstants;


out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    gl_Position = pushconstants.rendermatrix * vec4(vposition, 1.0f);
 
    outnormal = mat3(transpose(inverse( cameraData.model ))) * vnormal;
    outcoord = vuv;
    outpos = cameraData.model * vec4(vposition, 1.0);
}
