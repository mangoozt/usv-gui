#ifndef USV_GUI_GLRESTRICTIONS_H
#define USV_GUI_GLRESTRICTIONS_H

#include "usvdata/Restrictions.h"
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <utility>

class GLRestrictions {

public:
    GLRestrictions();

    ~GLRestrictions();

    void load_restrictions(const USV::Restrictions::Restrictions& restrictions);

    void render(QMatrix4x4& view_matrix, QVector3D eyePos);

private:
    using Point = std::array<GLfloat, 2>;
    using Index = GLuint;

    class Polygon {
        QOpenGLBuffer* vbo;
        QOpenGLBuffer* ibo;
        size_t indices_count;
        QVector3D color;
    public:
        Polygon(const USV::Restrictions::Polygon& polygon, const QVector3D& color);

        Polygon(Polygon&& o) noexcept:
                vbo(std::exchange(o.vbo, nullptr)), ibo(std::exchange(o.ibo, nullptr)), indices_count(o.indices_count)
                , color(o.color) {}

        ~Polygon();

        void render(QOpenGLShaderProgram* m_program);
    };

    class Isle {
        QOpenGLBuffer* vbo;
        QOpenGLBuffer* ibo;
        size_t indices_count;
        QVector3D color;
    public:
        Isle(const USV::Restrictions::Polygon& polygon, const QVector3D& color);

        Isle(Isle&& o) noexcept:
                vbo(std::exchange(o.vbo, nullptr)),
                ibo(std::exchange(o.ibo, nullptr)),
                indices_count(o.indices_count),
                color(o.color) {}

        ~Isle();

        void render(QOpenGLShaderProgram* m_program);
    };

    class Contour {
        QOpenGLBuffer* vbo;
        std::vector<GLuint> start_ptrs;
        QVector3D color;
    public:
        Contour(const USV::Restrictions::Polygon& polygon, const QVector3D& color);

        Contour(Contour&& o) noexcept:
                vbo(std::exchange(o.vbo, nullptr)),
                start_ptrs(std::move(o.start_ptrs)),
                color(o.color) {}

        ~Contour(){
            delete vbo;
        };

        void render(QOpenGLShaderProgram* m_program);
    };

    QOpenGLShaderProgram* m_program;
    int m_viewMatrixLoc;
    int m_viewLoc;
    int m_timeLoc;
    std::vector<Isle> glpolygons;
    std::vector<Contour> glcontours;
};


#endif //USV_GUI_GLRESTRICTIONS_H
