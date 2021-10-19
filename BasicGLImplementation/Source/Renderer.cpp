#include "Renderer.h"

#include "GL/glew.h"

#include <iostream>

// Use glGetError to clear all existing errors
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// Use glGetError to get current errors
bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR] - " << error << "'Function name : " << function << " File name : " << file << " Line number : " << line << std::endl;
        return false;
    }
    return true;
}