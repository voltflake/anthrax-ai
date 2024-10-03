#extension GL_EXT_nonuniform_qualifier : enable

#define BindlessDescriptorSet 0

#define BindlessUniformBinding 0
#define BindlessStorageBinding 1
#define BindlessSamplerBinding 2

#define GetLayoutVariableName(Name) u##Name##Register

#define RegisterUniform(Name, Struct) \
    layout(set = BindlessDescriptorSet, binding = BindlessUniformBinding) \
        uniform Name Struct \
        GetLayoutVariableName(Name)[]

#define RegisterBuffer(Layout, BufferAccess, Name, Struct) \
    layout(Layout, set = BindlessDescriptorSet, binding = BindlessStorageBinding) \
    BufferAccess buffer Name Struct GetLayoutVariableName(Name)[]

#define GetResource(Name, Index) \
    GetLayoutVariableName(Name)[Index]

RegisterUniform(DummyUniform, {uint ignore; });
RegisterBuffer(std430, readonly, DummyBuffer, { uint ignore; });

layout(set = BindlessDescriptorSet, binding = BindlessSamplerBinding) \
    uniform sampler2D textures[];

 
// const int MAX_BONES = 200;
// const int BONE_INFLUENCE = 4;
// #define DEPTH_ARRAY_SCALE 512
// #define MAX_POINT_LIGHTS 4

layout( push_constant ) uniform constants
{
    mat4 model;    
    mat4 rendermatrix;    

    int bindtexture;
    int bindbuffer;
} pushconstants;


RegisterUniform(Camera, {
    vec4 viewpos;
    vec4 mousepos;
    vec4 viewport;
    
    mat4 view;
    mat4 proj;
    mat4 viewproj;
});


// layout(set = 0, binding = 0) uniform  CameraBuffer {
// 	vec4 viewpos;
//     vec4 mousepos;
//     vec4 viewport;

// 	// vec4 dir_light_pos;
// 	// vec4 dir_light_dir;
//     // vec4 dir_light_color;
//     // vec4 point_light_pos[MAX_POINT_LIGHTS];
//     // vec4 point_light_dir[MAX_POINT_LIGHTS];
//     // vec4 point_light_color[MAX_POINT_LIGHTS];

//     mat4 view;
//     mat4 proj;
//     mat4 viewproj;

//     // float ambient;
// 	// float diffuse;
// 	// float specular;
    
//     // int hasdirectional;
//     // int pointlightamoount;
// } cameradata;