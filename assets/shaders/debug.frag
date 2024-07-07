#version 450

#include "helpers.h"

layout (location = 0) in vec3 innear;
layout(location = 1) in vec3 infar;

layout (location = 0) out vec4 outfragcolor;

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
} cameraData;

vec4 grid(vec3 pos, float scale, float div, vec3 col) {
    vec2 coord = pos.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);

    vec4 color = vec4(col, 1.0 - min(line, 1.0));
    // z axis
    if(pos.x > -0.1 * minimumx && pos.x < 0.1 * minimumx ) {
        color.b = 1.0;
    }
    // x axis
    if(pos.z > -0.1 * minimumz && pos.z < 0.1 * minimumz) {
        color.r = 1.0;
    }
    color.ga *= div;
    return color;
}


void main()
{
    float y = -innear.y / (infar.y - innear.y); // parametric equation of a line (y axis)
    vec3 clippos = innear + y * (infar - innear);

    if (y < 0) {
        outfragcolor = vec4(0);
        return ;
    }
    gl_FragDepth = ClipSpaceDepth(clippos, cameraData.proj, cameraData.view);

    float lineardepth = LinearDepth(clippos, cameraData.proj, cameraData.view);
    float fading = smoothstep(1, 0, lineardepth);

    outfragcolor.rgba = (grid(clippos, 1, 1, vec3(0.6, 0.6, 0.6))).rgba;
    outfragcolor.rgba += (grid(clippos, 10, 0.2, vec3(0.005, 0.005, 0.005))).rgba;
    outfragcolor.a *= fading;

}