const int MAX_BONES = 200;
const int BONE_INFLUENCE = 4;
#define DEPTH_ARRAY_SCALE 512

layout( push_constant ) uniform constants
{
    mat4 model;    
    mat4 rendermatrix;    
    int objectID;
    int boneind;
    int debug;
	int debugbones;
} pushconstants;


#define MAX_POINT_LIGHTS 4

layout(set = 0, binding = 0) uniform  CameraBuffer {
	vec4 viewpos;
    vec4 mousepos;
    vec4 viewport;

	// vec4 dir_light_pos;
	// vec4 dir_light_dir;
    // vec4 dir_light_color;
    // vec4 point_light_pos[MAX_POINT_LIGHTS];
    // vec4 point_light_dir[MAX_POINT_LIGHTS];
    // vec4 point_light_color[MAX_POINT_LIGHTS];

    mat4 view;
    mat4 proj;
    mat4 viewproj;

    // float ambient;
	// float diffuse;
	// float specular;
    
    // int hasdirectional;
    // int pointlightamoount;
} cameradata;