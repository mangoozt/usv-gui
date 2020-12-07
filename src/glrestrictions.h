#ifndef USV_GUI_GLRESTRICTIONS_H
#define USV_GUI_GLRESTRICTIONS_H

#include "usvdata/Restrictions.h"
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

class GLRestrictions {

public:
    GLRestrictions();

    ~GLRestrictions();

    void load_restrictions(const USV::Restrictions::Restrictions& restrictions);

    void render(QMatrix4x4& view_matrix, QVector3D eyePos);

private:

    class Polygon {
        QOpenGLBuffer* vbo;
        QOpenGLBuffer* ibo;
        size_t indices_count;
    public:
        explicit Polygon(const USV::Restrictions::Polygon&);

        Polygon(Polygon&& o) noexcept:
                vbo(std::exchange(o.vbo, nullptr)), ibo(std::exchange(o.ibo, nullptr)), indices_count(o.indices_count) {}

        ~Polygon();

        void render(QOpenGLShaderProgram* m_program);
    };

    QOpenGLShaderProgram* m_program;
    int m_viewMatrixLoc;
    int m_viewLoc;
    int m_timeLoc;
    std::vector<Polygon> glpolygons;
};


#endif //USV_GUI_GLRESTRICTIONS_H
