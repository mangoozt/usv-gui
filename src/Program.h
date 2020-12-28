#ifndef USV_GUI_PROGRAM_H
#define USV_GUI_PROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Program {
    int program;
    int vertexShader;

    std::vector<GLint> fragmentShaders;

public:

    Program();

    Program(const Program& other) = delete;

    Program(Program&& other) = delete;

    Program& operator=(const Program& other) = delete;

    Program& operator=(Program&& other) noexcept;

    void addVertexShader(const char* source);

    void addFragmentShader(const char* source);

    void link();

    void bind() const;

    static void release();

    GLint uniformLocation(const char* uniform) const;

    [[nodiscard]] inline GLint programId() const { return program; }

    void setUniformValue(GLint location, const GLfloat & f) const;

    void setUniformValue(GLint location, const glm::vec3& vec3) const;

    void setUniformValue(GLint location, const glm::vec4& vec4) const;

    void setUniformValue(GLint location, const glm::mat3& mat3) const;

    void setUniformValue(GLint location, const glm::mat4& mat4) const;

    ~Program();
};

#endif //USV_GUI_PROGRAM_H
