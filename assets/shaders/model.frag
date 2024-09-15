#version 450

#include "fragdef.h"
#include "defines.h"

layout (location = 4) in vec4 inweight;
layout (location = 5) flat in ivec4 inboneid;

layout(set = 2, binding = 0) buffer writeonly StorageArray
{
    mat4 bonesmatrices[MAX_BONES];
    uint data[DEPTH_ARRAY_SCALE];
} storagedata;


vec3 CalculateLight(vec3 light_pos, vec3 light_color, bool ispoint)
{
    vec3 norm = normalize(innormal.xyz);

    vec3 lightdir = normalize(light_pos - inpos.xyz);  

    float ndotl = max(dot(norm, lightdir), 0.0);
    vec3 diffuse = ndotl * (ispoint ? vec3(1) : light_color.xyz);

    vec3 color = texture(texturesampler ,incoord.xy).xyz;

    float ambientstrength = cameradata.ambient;
    vec3 ambient = ambientstrength * (ispoint ? vec3(1) : light_color.xyz);

    float specularStrength = cameradata.specular;
    vec3 viewdir = normalize(cameradata.viewpos.xyz - inpos.xyz);
    vec3 reflectdir = reflect(-lightdir, norm);
    float spec = pow(max(dot(viewdir, reflectdir), 0.0), 0);
    vec3 specular = specularStrength * spec * light_color.xyz;
    if (ispoint) {
     
        float dist = length(light_pos - inpos.xyz);
        float distsq = (dist) * (dist) * 2;
        float attenuation = 1.0 / distsq; // distance squared
      
        vec3 directionToLight = normalize(light_pos - inpos.xyz);

        float cosAngIncidence = max(dot(norm, directionToLight), 0);
        vec3 intensity = light_color.xyz * attenuation * 2.0f;
        diffuse *= intensity * cosAngIncidence;
        specular *= attenuation;
    }
    vec3 result = (ambient + diffuse + specular) * color;

    return result;
}

void main()
{
    if (pushconstants.objectID == 500) {
    	outfragcolor = vec4(cameradata.dir_light_color);
        return;
    }
    else if (pushconstants.objectID > 500) {
        int ind = pushconstants.objectID - 500;
        outfragcolor = vec4(cameradata.point_light_color[ind - 1]);
        return;
    }

    vec3 result = vec3(0);
    if (cameradata.hasdirectional == 1) {
        result += CalculateLight(cameradata.dir_light_pos.xyz, cameradata.dir_light_color.xyz, false);
    }
    for (int i = 0; i < cameradata.pointlightamoount; i++) {
        result += CalculateLight(cameradata.point_light_pos[i].xyz, cameradata.point_light_color[i].xyz, true);
    }

    uint zIndex = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);

    if(length( cameradata.mousepos.xy - gl_FragCoord.xy) < 1.0)
    {
        storagedata.data[zIndex] = pushconstants.objectID;
    }
    if (pushconstants.debug == 1) {
        result += vec3(0.5,0,0);
        clamp(result.r, 0, 1);
    }
    if (pushconstants.debugbones == 1) {
        for (int i = 0; i < 4; i++) {
          if (pushconstants.boneind == inboneid[i]) {
                if (inweight[i] >= 0.7) {
                    result = vec3(1, 0, 0) * inweight[i];
                }
                else if (inweight[i] >= 0.4 && inweight[i] <= 0.6) {
                    result = vec3(0, 1, 0) * inweight[i];
                }
                else if (inweight[i] >= 0.1) {
                    result = vec3(1, 1, 0) * inweight[i];
                }
          }
        }
    }
	outfragcolor = vec4(result,1.0f);
}