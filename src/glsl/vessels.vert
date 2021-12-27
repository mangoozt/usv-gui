#version 330
#define highp
#define mediump
#define lowp

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 position;
layout(location = 3) in float w;
layout(location = 4) in float scale;
layout(location = 5) in vec3 col;
out highp mat3 TBN;

out highp VERTEX_OUT{
    vec3 FragPos;
} vertex_out;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

out Material material;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    mat4 proj_view;
};

void main() {
    mat4 rot = mat4(cos(w),sin(w),0,0, -sin(w),cos(w),0,0, 0,0,1,0, 0,0,0,1);
    mat4 translate = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, position.x,position.y,position.z,1);
    mat4 m_scale = mat4(scale,0,0,0, 0,scale,0,0, 0,0,scale,0, 0,0,0,1);

    material.ambient = col*0.9;
    material.diffuse = col;
    material.specular = vec3(0.63, 0.63, 0.63);
    material.shininess = 32;

    gl_Position = proj_view * (translate*rot*m_scale*vertex);
    vec3 Normal = normalize(mat3(rot*m_scale)*normal);
    vec3 Tangent = normalize(vec3(Normal.z, 0, -Normal.y));
    vec3 Tangent2 = normalize(vec3(0, Normal.z, -Normal.x));
    TBN = mat3(Tangent, Tangent2, Normal);
    vertex_out.FragPos=vertex.xyz;
}
