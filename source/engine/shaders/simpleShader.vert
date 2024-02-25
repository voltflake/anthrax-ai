#version 450
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vUv;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outCoord;

layout(set = 0, binding = 0) uniform  CameraBuffer{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec2 pos;
    vec2 viewport;
} cameraData;

// //push constants block
// layout( push_constant ) uniform constants
// {
//  vec4 data;
//  mat4 render_matrix;
//  bool debugcollision;
// } PushConstants;


out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    float halfWidth = cameraData.viewport.x / 2.0f;//1920 / 2.0f;
    float halfHeight = cameraData.viewport.y / 2.0f;//1080 / 2.0f;

  //  mat4 transformMatrix = (PushConstants.render_matrix);
    gl_Position = vec4(vPosition.x / halfWidth - 1.0f, vPosition.y / halfHeight - 1.0f, 0.0, 1.0);
    outColor = vColor;
    outCoord = vUv;
}
