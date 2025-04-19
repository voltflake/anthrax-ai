#version 450

#include "defines/defines.h"
#include "defines/fragdef.h"

void main()
{

    vec3 light_pos[3] = { vec3(0.5, -3, -6), vec3(1, 1, 1), vec3(11, 0 , 3) };
    vec3 light_color[3] = { vec3(1), vec3(1,0, 0), vec3(0,1,0) };

    vec2 uv = incoord.xy;
    uv.y *= -1.0;
    vec4 normal = texture(textures[GetTextureInd()], uv.xy).xyzw;
    vec4 position = texture(textures[GetTextureInd() + 1], uv.xy).xyzw;

    vec4 albedo = texture(textures[GetTextureInd() + 2], uv.xy).xyzw;

    float ambient = 1.0;
    vec4 color = albedo * ambient;


    for (int i = 0 ; i < 3; i++) {
        vec3 lightdir = normalize(light_pos[i] - position.xyz);
        vec3 diffuse = dot(normal.xyz, lightdir) * albedo.xyz * light_color[i];
        color.xyz += diffuse;
    }


    outfragcolor = vec4(color.xyz, 1);
}
