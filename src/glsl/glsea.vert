#version 330
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 position;
layout(location = 4) in float scale;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

//struct LightSource {
//    vec4 position;
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//};

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
    mat4 translate = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, position.x, position.y, position.z, 1);
    mat4 m_scale = mat4(scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, 1);
    vec4 v = (translate*m_scale*vertex);
    v.z += cos((vertex.x+time)*0.5)+cos((vertex.x+time)*0.05)+sin((vertex.y+time)*0.7)+sin((vertex.y+time)*0.02);
    v.z=v.z*0.05;
    gl_Position = projection * view * v;
    vec3 Normal = vec3(0.05*vec2(0.5*sin((vertex.x+time)*0.5)+0.05*sin((vertex.x+time)*0.05), -0.7*cos((vertex.y+time)*0.7)-0.02*cos((vertex.y+time)*0.02)), 1);

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
