#ifndef TEXT_H
#define TEXT_H
#include <QFile>
#include <QHash>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>

class Text : protected QOpenGLExtraFunctions
{
public:
    struct Glyph{
        char character;  // UNICODE number of your char (codepage varies and may be specified before export). For example, 32 is a 'space'
        int xpos;   // x position of glyph on texture
        int ypos;   // y position of glyph on texture
        int width;  // width of glyph on texture (glyphs are cropped and Width and Orig Width aren't equal)
        int height;  // height of glyph on texture
        int xoffset; // distance on the x-axis, on which glyph must be shifted
        int yoffset; // distance on the y-axis, on which glyph must be shifted
        int orig_w; // original width of glyph
        int orig_h; // original height of glyph

        Glyph(QString string);
    };

    Text(QFile & fontfile, QImage& texture);
    ~Text();
    void renderText(std::string text, QPointF position, QRect window, QVector3D color={0,0,0}, float angle=0);
private:
    QHash<char,Glyph> glyphs;
    QOpenGLTexture texture;
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer *text_vbo;
    int m_projMatrixLoc;
    int m_colorLoc;
    float invW, invH;
};

#endif // TEXT_H
