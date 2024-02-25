#version 450

layout (location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUv;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform  CameraBuffer{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec2 pos;
    vec2 viewport;
} cameraData;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

//push constants block
layout( push_constant ) uniform constants
{
//  vec4 data;
//  mat4 render_matrix;
    int test;
    int debugcollision;
} PushConstants;

void main()
{
    outFragColor =  texture(texSampler, inUv).xyzw;
    if (PushConstants.debugcollision == 1) {
        float aspect = textureSize(texSampler, 0).x > textureSize(texSampler, 0).y ? textureSize(texSampler, 0).y / textureSize(texSampler, 0).x : textureSize(texSampler, 0).x / textureSize(texSampler, 0).y;
      //  if (textureSize(texSampler, 0).x / textureSize(texSampler, 0).y < textureSize(texSampler, 0).y / textureSize(texSampler, 0).x)
        if (inUv.x > 0.9*(aspect) || inUv.x < 0.1*aspect) {
        //if (inUv.x * aspect  < 0.02f || inUv.x > 0.98f  || inUv.y < 0.02f || inUv.y > 0.98f ) {
            outFragColor.rgb = vec3(0, 1, 0);
        }
    }
}