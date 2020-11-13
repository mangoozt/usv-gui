#include "text.h"
#include <QTextStream>
#include <QOpenGLContext>

static const char *vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec4 vertex;\n"
        "out vec2 TexCoords;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "   gl_Position = projection*vec4(vertex.xy, 0.0, 1.0);\n"
        "   TexCoords = vertex.zw;\n"
        "}\n";

static const char *fragmentShaderSource =
        "#version 330\n"
        "in vec2 TexCoords;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "out highp vec4 color;\n"
        "void main() {\n"
        "   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "   color = vec4(textColor, 1.0) * sampled;\n"
        "}\n";

Text::Glyph::Glyph(QString string){
    auto split = string.split('\t');
    character = char(split[0].toInt());
    xpos = split[1].toInt();
    ypos = split[2].toInt();
    width = split[3].toInt();
    height = split[4].toInt();
    xoffset = split[5].toInt();
    yoffset = split[6].toInt();
    orig_w = split[7].toInt();
    orig_h = split[8].toInt();
}

Text::Text(QFile & fontfile, QImage& bitmap):texture(bitmap)
{
    texture.setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
    invH=1.0f/bitmap.height();
    invW=1.0f/bitmap.width();
    if (fontfile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&fontfile);
        // skip font description
        in.readLine();
        in.readLine();
        while (!in.atEnd())
        {
            auto line = in.readLine();
            auto gl = Glyph(line);
            this->glyphs.insert(gl.character,gl);
        }
        fontfile.close();
    }
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projection");
    m_colorLoc = m_program->uniformLocation("textColor");

    m_program->release();
    text_vbo = new QOpenGLBuffer;
    text_vbo->create();

}


void Text::renderText(std::string text, QPoint position, QRect window, QVector3D color, float angle){
    auto pos = QVector3D((position.x()*2.0)/window.width(),(-position.y()*2.0)/window.height(),0);
    QMatrix4x4 proj_mat;
    proj_mat.translate(pos);
    proj_mat.ortho(0,window.width(),window.height(),0,1,-1);
    proj_mat.rotate(angle,0,0,1.0f);

    auto *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    texture.bind();
    text_vbo->bind();

    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof (GLfloat),0);
    m_program->setUniformValue(m_projMatrixLoc,proj_mat);
    m_program->setUniformValue(m_colorLoc,QVector3D(color));

    auto x= 0.0f;
    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Glyph ch = glyphs.find(*c).value();

        GLfloat cxl = x + ch.xoffset;
        GLfloat cxr = cxl + ch.width;
        GLfloat cyt = ch.yoffset;
        GLfloat cyb = cyt+ch.height;

        GLfloat txl = ch.xpos*invW;
        GLfloat txr = (ch.xpos+ch.width)*invW;
        GLfloat tyt = ch.ypos*invH;
        GLfloat tyb = (ch.ypos+ch.height)*invH;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { cxl, cyt, txl, tyt },
            { cxl, cyb, txl, tyb },
            { cxr, cyb, txr, tyb },
            { cxr, cyt, txr, tyt }
        };
        text_vbo->allocate(vertices,sizeof (vertices));
        f->glDrawArrays(GL_TRIANGLE_FAN,0,4);

        x += ch.orig_w;
    }
    m_program->release();
}



Text::~Text(){
    delete m_program;
}
