#ifndef USV_GUI_BUFFER_H
#define USV_GUI_BUFFER_H
#include <glad/glad.h>

class Buffer {
    GLuint buffer;
public:
    [[nodiscard]] inline GLuint bufferId() const { return buffer; }
    void create();
    void bind() const;
    static void allocate(const GLvoid * data, GLsizeiptr size);
    static void release();
};

#endif //USV_GUI_BUFFER_H
