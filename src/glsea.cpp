#include <cmath>
#include <iostream>
#include "glsea.h"
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

static const char *vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec4 vertex;\n"
        "layout(location = 1) in vec4 position;\n"
        "layout(location = 4) in float scale;\n"
        "struct Light {"
        "   vec3 position;"
        "   vec3 ambient;"
        "   vec3 diffuse;"
        "   vec3 specular;"
        "};"
        "uniform float time;\n"
        "out highp mat3 TBN;"
        "uniform mat4 m_view;\n"
        "uniform highp vec3 viewPos;\n"
        "uniform Light light; "
        "out highp VERTEX_OUT{"
        "   vec3 highp FragPos;"
        "   vec2 highp TextCoords;"
        "   vec3 highp TangentLightPos;"
        "   vec3 highp TangentViewPos;"
        "   vec3 highp TangentFragPos;"
        "} vertex_out;"
        "void main() {\n"
        "   mat4 translate = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, position.x,position.y,position.z,1);\n"
        "   mat4 m_scale = mat4(scale,0,0,0, 0,scale,0,0, 0,0,scale,0, 0,0,0,1);\n"
        "   vec4 v = (translate*m_scale*vertex);\n"
        "   v.z += cos((vertex.x+time)*0.5)+cos((vertex.x+time)*0.05)+sin((vertex.y+time)*0.7)+sin((vertex.y+time)*0.02);\n"
        "   v.z=v.z*0.05+0.1;"
        "   gl_Position = m_view * v;\n"
        "   vec3 Normal = vec3(0.05*vec2(0.5*sin((vertex.x+time)*0.5)+0.05*sin((vertex.x+time)*0.05), -0.7*cos((vertex.y+time)*0.7)-0.02*cos((vertex.y+time)*0.02)), 1);\n"
        "   Normal = normalize(Normal);\n"
//        "   Normal = normalize(vec3(1,0,1));\n"
        "   vec3 Tangent = normalize(vec3(Normal.z,0,-Normal.y));"
        "   vec3 Tangent2 = normalize(vec3(0,Normal.z,-Normal.x));"
        "   TBN = transpose(mat3(Tangent,Tangent2,Normal));"
        "   vertex_out.FragPos = v.xyz;"
        "   vertex_out.TangentLightPos = TBN*light.position;\n"
        "   vertex_out.TangentViewPos = TBN*viewPos;\n"
        "   vertex_out.TangentFragPos = TBN*v.xyz;\n"
        "   vertex_out.TextCoords = v.xy*2 + 2*vec2(cos((time)*0.04), sin((time)*0.02));\n"
        "TBN=transpose(TBN);"
        "}\n";

static const char *fragmentShaderSource =
        "#version 330\n"
//        "#extension GL_OES_standard_derivatives : enable\n"
        "struct Material {"
        "   vec3 ambient;"
        "   vec3 diffuse;"
        "   vec3 specular;"
        "   float shininess;"
        "};"
        "struct Light {"
        "   vec3 position;"
        "   vec3 ambient;"
        "   vec3 diffuse;"
        "   vec3 specular;"
        "};"
        "in highp mat3 TBN;\n"
        "in highp vec3 Pos;\n"
        "out highp vec4 fragColor;\n"
        "uniform Light light; "
        "uniform highp vec3 viewPos;\n"
        "uniform float height_scale;"
        "uniform Material material;\n"
        "uniform sampler2D tex_normal;\n"
        "uniform sampler2D depthMap;\n"
        "uniform sampler2D specularMap;\n"
        "in VERTEX_OUT{"
        "   vec3 highp FragPos;"
        "   vec2 highp TextCoords;"
        "   vec3 highp TangentLightPos;"
        "   vec3 highp TangentViewPos;"
        "   vec3 highp TangentFragPos;"
        "} vertex_out;"
        "vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);\n"
        ""
        "void main() {\n"
        "   vec3 viewDir   = normalize(vertex_out.TangentViewPos - vertex_out.TangentFragPos);\n"
        "   vec2 texCoords = ParallaxMapping(vertex_out.TextCoords,  viewDir);"
        "   vec3 texnorm = texture(tex_normal, texCoords).xyz*2.0-1.0;\n"
        "   vec3 norm = normalize(TBN*texnorm);"
        // ambient
        "   vec3 ambient = light.ambient * material.ambient;\n"
        // diffuse
        "   vec3 lightDir = normalize(light.position - vertex_out.FragPos);\n"
        "   float diff = max(dot(norm, lightDir), 0.0);\n"
        "   vec3 diffuse = light.diffuse * (diff * material.diffuse*texture(depthMap, texCoords).r);\n"
        // specular
        "   viewDir = normalize(viewPos - vertex_out.FragPos);\n"
        "   vec3 reflectDir = reflect(-lightDir, norm);\n"
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
        "   vec3 specular = light.specular * (spec * material.specular*texture(specularMap,texCoords).r);\n"
        "   vec3 result = ambient + diffuse + specular;\n"
        "   fragColor = vec4(result, 0.8+diff);\n"
//        "   fragColor = vec4(norm,1.0);"
        "}\n"
        "vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)"
        "{\n"
        "   float height =  texture(depthMap, texCoords).r;\n"
        "   vec2 p = viewDir.xy / viewDir.z * (height * height_scale);\n"
        "   return texCoords - p;\n"
        "}"
        "\n";

GLSea::GLSea(QImage& texture, QImage& normal, QImage &specular): tex(texture),normal_tex(normal),spec_tex(specular)
{
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_program->bind();
    m_viewMatrixLoc = m_program->uniformLocation("m_view");
    m_viewLoc = m_program->uniformLocation("viewPos");
    m_timeLoc = m_program->uniformLocation("time");
    m_program->setUniformValue(m_program->uniformLocation("height_scale"), 0.2f);
    struct {
        QVector3D position=QVector3D(-100.0,100.0,10.0);
        QVector3D ambient=QVector3D(0.7, 0.6, 0.6);
        QVector3D diffuse=QVector3D(0.7, 0.6, 0.6);
        QVector3D specular=QVector3D(1, 1, 1);
    } light;
    m_program->setUniformValue(m_program->uniformLocation("light.position"), light.position);
    m_program->setUniformValue(m_program->uniformLocation("light.ambient"), light.ambient);
    m_program->setUniformValue(m_program->uniformLocation("light.diffuse"), light.diffuse);
    m_program->setUniformValue(m_program->uniformLocation("light.specular"), light.specular);

    struct {
        QVector3D ambient=QVector3D(127,205,255)/255;
        QVector3D diffuse=QVector3D(127,205,255)/255;
        QVector3D specular=QVector3D(255, 204, 51)/400;
        float shininess{256};
    } material;
    m_program->setUniformValue(m_program->uniformLocation("material.ambient"), material.ambient);
    m_program->setUniformValue(m_program->uniformLocation("material.diffuse"), material.diffuse);
    m_program->setUniformValue(m_program->uniformLocation("material.specular"), material.specular);
    m_program->setUniformValue(m_program->uniformLocation("material.shininess"), material.shininess);
    f->glUniform1i(m_program->uniformLocation("tex_normal"),0);
    f->glUniform1i(m_program->uniformLocation("depthMap"),2);
    f->glUniform1i(m_program->uniformLocation("specularMap"),4);
    m_program->release();
    vbo = new QOpenGLBuffer();
    ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    vbo->create();
    ibo->create();
    prepare_grid();
}

GLSea::~GLSea()
{
    delete m_program;
    delete vbo;
    delete ibo;
}

void GLSea::render(QMatrix4x4 &view_matrix, QVector3D eyePos, float time)
{
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glDepthMask(GL_FALSE);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    normal_tex.bind(0);
    tex.bind(2);
    spec_tex.bind(4);
    m_program->setUniformValue(m_viewMatrixLoc, view_matrix);
    m_program->setUniformValue(m_timeLoc, (time*10));
    m_program->setUniformValue(m_viewLoc, eyePos);
    vbo->bind();
    ibo->bind();
    int vertexLocation = m_program->attributeLocation("vertex");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, GL_FALSE, 2, 0);
    f->glDrawElements(GL_TRIANGLES, (gridsize-1)*(gridsize-1)*6, GL_UNSIGNED_INT, 0);
    ibo->release();
    m_program->release();
    f->glDisable(GL_BLEND);
    f->glDepthMask(GL_TRUE);
}

void GLSea::prepare_grid()
{
    std::vector<GLfloat> vertices;
    vertices.reserve(gridsize*gridsize*2);
    auto step = size/gridsize;
    auto zero = -size*0.5f;
    for(size_t i=0;i<gridsize;++i)
        for(size_t j=0;j<gridsize;++j){
            vertices.push_back(zero + i*step);
            vertices.push_back(zero + j*step);
        }

    std::vector<GLuint> indices;
    for(GLuint i=0;i<gridsize-1;++i)
        for(GLuint j=0;j<gridsize-1;++j){
            // first triangle
            auto r = i*gridsize;
            indices.push_back(r+j+gridsize);
            indices.push_back(r+j+1);
            indices.push_back(r+j);
            // second triangle
            indices.push_back(r+j+gridsize);
            indices.push_back(r+j+gridsize+1);
            indices.push_back(r+j+1);
        }

    vbo->bind();
    vbo->allocate(vertices.data(),sizeof (GLfloat)*vertices.size());
    vbo->release();
    ibo->bind();
    ibo->allocate(indices.data(),sizeof (GLuint)*indices.size());
    ibo->release();
}
