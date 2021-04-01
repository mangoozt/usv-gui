#version 330
#define highp
#define mediump
#define lowp

layout(location = 0) in vec4 vertex;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

layout (std140) uniform Light
{
    vec4 light_position;
    vec3 light_ambient;
    vec3 light_diffuse;
    vec3 light_specular;
};

uniform float time;
out highp mat3 TBN;
uniform highp vec3 viewPos;

out highp VERTEX_OUT{
    vec3 FragPos;
    vec2 TextCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vertex_out;

void main() {
    vec4 v = vec4(0, 0, 0, 1);
    mat4 screen_mat = projection * view;
    v = screen_mat * v;
    v/=v.w;
    v.xy = vertex.xy*1.1;
    v = inverse(screen_mat) * v;
    v /= v.w;
    // waves
    v.z += cos((v.x+time)*0.5)+cos((v.x+time)*0.05)+sin((v.y+time)*0.7)+sin((v.y+time)*0.02);
    v.z=v.z*0.05-0.1;
    gl_Position = screen_mat * v;
    gl_Position /= gl_Position.w;
    vec3 Normal = vec3(0.05*vec2(0.5*sin((v.x+time)*0.5)+0.05*sin((v.x+time)*0.05), -0.7*cos((v.y+time)*0.7)-0.02*cos((v.y+time)*0.02)), 1);

    Normal = normalize(Normal);
    vec3 Tangent = normalize(vec3(Normal.z, 0, -Normal.y));
    vec3 Tangent2 = normalize(vec3(0, Normal.z, -Normal.x));
    TBN = transpose(mat3(Tangent, Tangent2, Normal));
    vertex_out.FragPos = v.xyz;
    vertex_out.TangentLightPos = TBN*light_position.xyz;
    vertex_out.TangentViewPos = TBN*viewPos;
    vertex_out.TangentFragPos = TBN*v.xyz;
    vertex_out.TextCoords = v.xy*2 + 2*vec2(cos((time)*0.04), sin((time)*0.02));
    TBN=transpose(TBN);
}
