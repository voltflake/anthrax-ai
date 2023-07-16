#version 450

layout (location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUv;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform  CameraBuffer{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec2 pos;
} cameraData;

layout(set = 1, binding = 0) uniform sampler2D texSampler;


void main()
{
  float halfWidth = 1920;// / 2.0f;
    float halfHeight = 1080;// / 2.0f;
vec4 test = texture(texSampler, inUv).xyzw;;
	outFragColor = vec4(0,0,0,0);//

   //float n = (iMouse.z < .5)?100.0:1.0;

    vec2 mousepos = vec2(cameraData.pos.x / halfWidth , cameraData.pos.y / halfHeight);
     float d = length(inUv.xy-mousepos.xy);
    
    float a = 1.0-min(d / 0.05, 1.0);
    
    outFragColor = test + vec4(0.05,0.05,0.05,1) * a;
}