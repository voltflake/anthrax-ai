#version 450

#include "fragdef.h"

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

	outfragcolor = vec4(result,1.0f);
}