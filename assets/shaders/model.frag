#version 450

layout(location = 0) in vec4 fragpos;
layout (location = 1) in vec3 innormal;
layout(location = 2) in vec2 inuv;

layout (location = 0) out vec4 fragcolor;

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
} camdata;

layout(set = 1, binding = 0) uniform sampler2D texturesampler;

void main()
{
    vec3 norm = normalize(innormal);
    vec3 lightdir = normalize(camdata.lightpos.xyz - fragpos.xyz);  

    float ndotl = max(dot(norm, lightdir), 0.0);
    vec3 diffuse = ndotl * camdata.lightcolor.xyz;

    vec3 color = texture(texturesampler ,inuv).xyz;

    float ambientstrength = camdata.ambient;
    vec3 ambient = ambientstrength * camdata.lightcolor.xyz;

    float specularStrength = camdata.specular;
    vec3 viewdir = normalize(camdata.viewpos.xyz - fragpos.xyz);
    vec3 reflectdir = reflect(-lightdir, norm);
    float spec = pow(max(dot(viewdir, reflectdir), 0.0), 2);
    vec3 specular = specularStrength * spec * camdata.lightcolor.xyz; 

    vec3 result = (ambient + diffuse + specular) * color;

	fragcolor = vec4(result,1.0f);
}