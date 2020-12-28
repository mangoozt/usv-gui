#ifndef USV_GUI_GLRESTRICTIONS_H
#define USV_GUI_GLRESTRICTIONS_H

#include "usvdata/Restrictions.h"
#include <glm/glm.hpp>
#include "Program.h"
#include "Buffer.h"
#include <utility>
#include <memory>

class GLRestrictions {
    struct RestrictionMeta {
        const USV::FeatureProperties* properties;
    };
    std::vector<RestrictionMeta> meta_;
public:
    GLRestrictions();

    void load_restrictions(const USV::Restrictions::Restrictions& restrictions);

    typedef unsigned int GeometryType;
    struct GeometryTypes {
        static const GeometryType Contour = 1;
        static const GeometryType Polygon = 2;
        static const GeometryType Isle = 4;
        static const GeometryType All = 7;
    };

    void render(glm::mat4& view_matrix, glm::vec3 eyePos, GeometryType gtype = GeometryTypes::All);

private:
    using Point = std::array<GLfloat, 2>;
    using Index = GLuint;

    class Polygon {
        std::unique_ptr<Buffer> vbo;
        std::unique_ptr<Buffer> ibo;
        GLsizei indices_count;
        glm::vec3 color;
        size_t id_;
        float opacity;
    public:
        Polygon(const USV::Restrictions::Polygon& polygon, const glm::vec3& color, size_t id, float opacity = 1.0);

        Polygon(Polygon&& o) noexcept:
                vbo(std::exchange(o.vbo, nullptr)), ibo(std::exchange(o.ibo, nullptr)), indices_count(o.indices_count)
                , color(o.color), opacity(o.opacity), id_(o.id_) {};

        void render(const Program& m_program);
    };

    class Isle {
        std::unique_ptr<Buffer> vbo;
        std::unique_ptr<Buffer> ibo;
        GLuint indices_count;
        glm::vec3 color;
        size_t id_;
    public:
        Isle(const USV::Restrictions::Polygon& polygon, const glm::vec3& color, size_t id);

        Isle(Isle&& o) noexcept:
                vbo(std::exchange(o.vbo, nullptr)), ibo(std::exchange(o.ibo, nullptr)), indices_count(o.indices_count)
                , color(o.color), id_(o.id_) {}

        void render(const Program& m_program);
    };

    class Contour {
        std::unique_ptr<Buffer> vbo;
        std::vector<GLuint> start_ptrs;
        glm::vec3 color;
        size_t id_;
    public:
        Contour(const USV::Restrictions::Polygon& polygon, const glm::vec3& color, size_t id);

        Contour(Contour&& o) noexcept:
                vbo(std::exchange(o.vbo, nullptr)), start_ptrs(std::move(o.start_ptrs)), color(o.color), id_(o.id_) {}

        void render(const Program& m_program);
    };

    std::unique_ptr<Program> m_program;
    int m_viewLoc;
    std::vector<Isle> glisles;
    std::vector<Polygon> glpolygons;
    std::vector<Contour> glcontours;
};


#endif //USV_GUI_GLRESTRICTIONS_H
