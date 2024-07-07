layout (location = 0) in vec4 inpos;
layout (location = 1) in vec3 innormal;
layout (location = 2) in vec3 incolor;
layout (location = 3) in vec2 incoord;
// layout (location = 4) in vec4 inweight;
// layout (location = 5) in ivec4 inboneid;

layout (location = 0) out vec4 outfragcolor;

layout( push_constant ) uniform constants
{
    mat4 rendermatrix;

    int objectID;
    int boneind;
    int debug;
	int debugbones;
} pushconstants;


layout(set = 0, binding = 0) uniform  CameraBuffer {

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

layout(set = 1, binding = 0) uniform sampler2D texturesampler;

