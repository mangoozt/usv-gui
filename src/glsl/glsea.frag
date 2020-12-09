#version 330
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in highp mat3 TBN;
in highp vec3 Pos;
out highp vec4 fragColor;

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

uniform highp vec3 viewPos;
uniform float height_scale;
uniform Material material;
uniform sampler2D tex_normal;
uniform sampler2D depthMap;
uniform sampler2D specularMap;

in VERTEX_OUT{
    vec3 FragPos;
    vec2 TextCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vertex_out;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main() {
    vec3 viewDir   = normalize(vertex_out.TangentViewPos - vertex_out.TangentFragPos);
    vec2 texCoords = ParallaxMapping(vertex_out.TextCoords, viewDir);
    vec3 texnorm = texture(tex_normal, texCoords).xyz*2.0-1.0;
    vec3 norm = normalize(TBN*texnorm);
    // ambient
    vec3 ambient = light_ambient * material.ambient;
    // diffuse
    vec3 lightDir = normalize(light_position.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light_diffuse * (diff * material.diffuse*texture(depthMap, texCoords).r);
    // specular
    viewDir = normalize(viewPos - vertex_out.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light_specular * (spec * material.specular*texture(specularMap, texCoords).r);
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 0.8+diff);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height =  texture(depthMap, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}
