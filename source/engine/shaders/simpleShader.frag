#version 450

layout (location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUv;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform  CameraBuffer{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec4 color;
} cameraData;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
	outFragColor =  vec4(inColor, 1.0);//texture(texSampler, inUv);
}