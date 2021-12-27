#version 330
#define highp
#define mediump
#define lowp

layout(location = 0) in vec4 vertex;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    mat4 proj_view;
};

layout (std140) uniform Light
{
    vec4 light_position;
    vec3 light_ambient;
    vec3 light_diffuse;
    vec3 light_specular;
};

out highp mat3 TBN;
uniform highp vec3 viewPos;

out highp VERTEX_OUT{
    vec3 FragPos;
} vertex_out;

void main() {
    vec4 v = vec4(0, 0, 0, 1);
    v = proj_view * v;
    v/=v.w;
    v.xy = vertex.xy*1.1;
    v = inverse(proj_view) * v;
    v /= v.w;
    gl_Position = proj_view * v;
    gl_Position /= gl_Position.w;
    vec3 Normal = vec3(0, 0, 1);
    vec3 Tangent = normalize(vec3(Normal.z, 0, -Normal.y));
    vec3 Tangent2 = normalize(vec3(0, Normal.z, -Normal.x));
    TBN = transpose(mat3(Tangent, Tangent2, Normal));
    vertex_out.FragPos = v.xyz;
    TBN=transpose(TBN);
}
