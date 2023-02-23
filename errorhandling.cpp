#include <GLFW/glfw3.h>
#include <iostream>
#include "errorhandling.h"


void GlClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GlLogCall(const char* function, const char* file, int line)
{
    if (glGetError())
    {
        std::cout << "[OpenGL] (" << function << ") : " << file << " : " << line << std::endl;
        return false;
    }
    return true;
}