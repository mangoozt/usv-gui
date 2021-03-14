#ifndef USV_GUI_BUFFER_H
#define USV_GUI_BUFFER_H
#if defined(NANOGUI_GLAD)
#include <glad/glad.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif

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
