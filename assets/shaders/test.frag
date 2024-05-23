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
layout(set = 1, binding = 1) uniform sampler2D texSampler2;

float dist(vec2 p)
{ 
    vec2 q = vec2(p.x*1920/1080,p.y);
    return length(q);
}


void main()
{
  float halfWidth = 1920;// / 2.0f;
    float halfHeight = 1080;// / 2.0f;
vec4 test = texture(texSampler, inUv).xyzw;;

vec4 test2 = texture(texSampler2, inUv).xyzw;;


 vec2 mousepos = vec2(cameraData.pos.x / halfWidth , cameraData.pos.y / halfHeight);
     float d = dist(inUv.xy-mousepos.xy);
    
    //float a = (1 - min(d / 0.05, 1.0))/4;
    float a = 1 - smoothstep(d/ 0.8,0., 0.01);
     vec4 paintcol = vec4(a);

     if (cameraData.pos.x  != 0 && cameraData.pos.y != 0){
        outFragColor = test + (test2 * paintcol);//vec4(test2.r * a, test2.g * a, test2.b * a, 1);

     }
   else {
        outFragColor = test;
   }
}