#version 330
#define highp
#define mediump
#define lowp

layout (std140) uniform Light
{
    vec4 light_position;
    vec3 light_ambient;
    vec3 light_diffuse;
    vec3 light_specular;
};

in highp mat3 TBN;
layout(location = 0) out highp vec4 fragColor;
uniform highp vec3 viewPos;
uniform float opacity;
in highp VERTEX_OUT{
    vec3 FragPos;
} vertex_out;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in Material material;

void main() {
    vec3 norm = normalize(TBN[2]);
    // ambient
    vec3 ambient = light_ambient * material.ambient;
    // diffuse
    vec3 lightDir = normalize(light_position.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light_diffuse * (diff * material.diffuse);
    // specular
    vec3 viewDir = normalize(viewPos - vertex_out.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light_specular * (spec * material.specular);
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, opacity);
}