#version 450

#include "fragdef.h"
layout (location = 4) in vec4 inweight;
layout (location = 5) flat in ivec4 inboneid;

#define DEPTH_ARRAY_SCALE 512
const int MAX_BONES = 200;

layout(set = 2, binding = 0) buffer writeonly StorageArray
{
    mat4 bonesmatrices[MAX_BONES];
    uint data[DEPTH_ARRAY_SCALE];
} storagedata;

void main()
{
    vec3 norm = normalize(innormal.xyz);
    vec3 lightdir = normalize(cameradata.lightpos.xyz - inpos.xyz);  

    float ndotl = max(dot(norm, lightdir), 0.0);
    vec3 diffuse = ndotl * cameradata.lightcolor.xyz;

    vec3 color = texture(texturesampler ,incoord.xy).xyz;

    float ambientstrength = cameradata.ambient;
    vec3 ambient = ambientstrength * cameradata.lightcolor.xyz;

    float specularStrength = cameradata.specular;
    vec3 viewdir = normalize(cameradata.viewpos.xyz - inpos.xyz);
    vec3 reflectdir = reflect(-lightdir, norm);
    float spec = pow(max(dot(viewdir, reflectdir), 0.0), 2);
    vec3 specular = specularStrength * spec * cameradata.lightcolor.xyz; 

    vec3 result = (ambient + diffuse + specular) * color;

    uint zIndex = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);

    if( length( cameradata.mousepos.xy - gl_FragCoord.xy) < 1)
    {
        storagedata.data[zIndex] = pushconstants.objectID;
    }
    if (pushconstants.debug == 1) {
        result += vec3(0.2,0,0);
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