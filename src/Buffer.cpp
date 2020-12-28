#include "Buffer.h"

void Buffer::create() {
    glGenBuffers(1, &buffer);
}

void Buffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void Buffer::allocate(const GLvoid* data, GLsizeiptr size) {
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Buffer::release() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
