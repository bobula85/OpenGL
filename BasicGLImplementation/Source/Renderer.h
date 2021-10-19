#pragma once

#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x) GLClearError();\
                  x;\
                  ASSERT(GLLogCall(#x, __FILE__, __LINE__ ))


// Use glGetError to clear all existing errors
void GLClearError();

// Use glGetError to get current errors
bool GLLogCall(const char* function, const char* file, int line);
