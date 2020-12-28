#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "Program.h"
#include <iostream>

void printShaderLog(GLuint shader) {
    GLint i;
    char* s;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &i);
    if (i > 0) {
        s = (GLchar*) malloc(i);
        glGetShaderInfoLog(shader, i, &i, s);
        std::cerr << "compile log = '" << s << "'\n";
    }
}

void checkShader(GLuint s, const char* source) {
    GLint compiled;
    glGetShaderiv(s, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderLog(s);
        std::cerr << source << std::endl;
        exit(-1);
    }
}

int checkProgram(GLuint p) {
    GLint linked;
    glGetProgramiv(p, GL_LINK_STATUS, &linked);
    return linked;
}

void Program::addVertexShader(const char* source) {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glAttachShader(program, vertexShader);
    glShaderSource(vertexShader, 1, &source, nullptr);
    glCompileShader(vertexShader);
    checkShader(vertexShader, source);
    assert(glGetError() == 0);
}

void Program::addFragmentShader(const char* source) {
    fragmentShaders.push_back(glCreateShader(GL_FRAGMENT_SHADER));
    glShaderSource(fragmentShaders.back(), 1, &source, nullptr);
    glCompileShader(fragmentShaders.back());
    checkShader(fragmentShaders.back(), source);
    glAttachShader(program, fragmentShaders.back());
    assert(glGetError() == 0);
}

void Program::link() {
    glLinkProgram(program);
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        char* log = new char[logLength];
        glGetProgramInfoLog(program, logLength, &logLength, log);
        printf("%s", log);
    }

    if (checkProgram(program)) {
        assert(glGetError() == 0);
    } else {
        printShaderLog(vertexShader);
        for (auto fragmentShader: fragmentShaders)
            printShaderLog(fragmentShader);
        exit(-1);
    }
}

GLint Program::uniformLocation(const char* uniform) const {
    return glGetUniformLocation(program, uniform);
}

void Program::bind() const {
    glUseProgram(program);
}

void Program::release() {
    glUseProgram(0);
}

void Program::setUniformValue(GLint location, const glm::vec3& v) const {
    glUseProgram(program);
    glUniform3fv(location, 1, reinterpret_cast<const GLfloat*>(&v));
}

void Program::setUniformValue(GLint location, const glm::vec4& v) const {
    glUseProgram(program);
    glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(&v));
}

void Program::setUniformValue(GLint location, const glm::mat3& v) const {
    glUseProgram(program);
    glUniformMatrix3fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&v));
}

void Program::setUniformValue(GLint location, const glm::mat4& v) const {
    glUseProgram(program);
    glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&v));
}

void Program::setUniformValue(GLint location, const GLfloat& v) const {
    glUseProgram(program);
    glUniform1f(location, v);
}

Program::Program() : vertexShader(-1) {
    program = glCreateProgram();
    assert(glGetError() == 0);
}

Program::~Program() {
    glDeleteProgram(program);
    if (vertexShader != -1) {
        glDeleteShader(vertexShader);
    }
    for (auto fragmentShader:fragmentShaders)
        if (fragmentShader != -1) {
            glDeleteShader(fragmentShader);
        }
}

Program& Program::operator=(Program&& other) noexcept {
    glDeleteProgram(program);
    if (vertexShader != -1) {
        glDeleteShader(vertexShader);
    }
    for (auto fragmentShader:fragmentShaders)
        if (fragmentShader != -1) {
            glDeleteShader(fragmentShader);
        }
    std::swap(program, other.program);
    std::swap(vertexShader, other.vertexShader);
    std::swap(fragmentShaders, other.fragmentShaders);
    return *this;
}

