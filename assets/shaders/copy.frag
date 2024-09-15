#version 450
layout (location = 0) in vec4 inpos;
layout (location = 1) in vec3 innormal;
layout (location = 2) in vec3 incolor;
layout (location = 3) in vec2 incoord;

layout (location = 0) out vec4 outfragcolor;

#include "defines.h"

layout (input_attachment_index = 0, set = 1, binding = 1) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 2) uniform subpassInput inputDepth;

void main()
{
    vec3 color = subpassLoad(inputColor).rgb;

	outfragcolor = vec4(color,1.0f);
}