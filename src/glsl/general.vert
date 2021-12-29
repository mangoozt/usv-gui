#version 330
#define highp
#define mediump
#define lowp

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;
out highp mat3 TBN;
uniform mat4 m_view;
out highp VERTEX_OUT{
    vec3 FragPos;
} vertex_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    mat4 proj_view;
};

void main() {
    gl_Position = proj_view * vertex;
    vec3 Normal = normalize(normal);
    vec3 Tangent = normalize(vec3(Normal.z, 0, -Normal.y));
    vec3 Tangent2 = normalize(vec3(0, Normal.z, -Normal.x));
    TBN = mat3(Tangent, Tangent2, Normal);
    vertex_out.FragPos=vertex.xyz;
}
