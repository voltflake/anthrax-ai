#version 450

vec3 plane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    gl_Position = vec4(plane[gl_VertexIndex].xyz, 1.0);
}
