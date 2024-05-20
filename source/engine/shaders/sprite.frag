#version 450

layout (location = 0) in vec3 incolor;
layout(location = 1) in vec2 inuv;

layout (location = 0) out vec4 outfragcolor;

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
} cameraData;

layout(set = 1, binding = 0) uniform sampler2D spritesampler;

void main()
{
    outfragcolor = texture(spritesampler, inuv).xyzw;
}