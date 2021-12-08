
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

struct colourChangeValues
{
    float R;
    float G;
    float B;

    float Inc;
};

colourChangeValues colours{ 0.1f, 0.1f, 0.1f, 0.1f }; // Bad! Bad! Bad! Globals Very Bad!

// Takes a colour element Ref and increments or decrements it depending on colour element value 
void IncColour(float& colour)
{
    colour += (colour > 1.0f) ? colours.Inc = -0.01f : colours.Inc = 0.01f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        IncColour(colours.R);

    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        IncColour(colours.G);

    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        IncColour(colours.B);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1; 
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    glfwSwapInterval(1);

    GLenum err = glewInit();

    if (err != GLEW_OK)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "GLEW Init() Error: %s\n", glewGetErrorString(err);
        // fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        // ...
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Vertex array of positions
    float SimpleSquarePositions[] =         {-0.5f, -0.5f,
                                              0.5f, -0.5f,
                                              0.5f,  0.5f,
                                             -0.5f,  0.5f };

    // Index array of vertices
    unsigned int SimpleSquareIndices[] =   {0,1,2,
                                            2,3,0};


	// Vertex array objects link together vertex buffers and attribute into one object
	VertexArray vertexArray;

	// Construct VertexBuffer object 
	VertexBuffer vertexBuffer(SimpleSquarePositions, 4 * 2 * sizeof(float));

	VertexBufferLayout layout;
	layout.Push<float>(2);

	vertexArray.AddBuffer(vertexBuffer, layout);

    //vertexArrayObject
   
    // Vertex array objects link together vertex buffers and attribute into one object
    //unsigned int vertexArrayObject;
    //GLCall(glGenVertexArrays(1, &vertexArrayObject));
   // GLCall(glBindVertexArray(vertexArrayObject));

    // Construct VertexBuffer object 
   // VertexBuffer vertexBuffer(SimpleSquarePositions, 4 * 2 * sizeof(float));


    // Enable the vertex attribute Array
    GLCall(glEnableVertexAttribArray(0));

    // Set vertex attribute details 
    // This call links the currently bound vertex buffer (at 0 as per glVertexAttribPointer(0... <-- ) 
    // and attribute in the vertex array object above. The vertexArrayObject can then be bound and used instead of bind buffer and glVertexAttribPointer
    //GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    IndexBuffer indexBuffer(SimpleSquareIndices, 6);

    Shader shader("Res/Shaders/BasicShader.shader");
    shader.Bind();

    shader.SetUniform4f("u_Colour", 0.0f, 1.0f, 0.0f, 1.0f);

    // Unbind all buffers/programs/attribs by passing 0

    vertexArray.Unbind();
    shader.Unbind();
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        // Bind the shader program
        shader.Bind();

        // Set the colour uniform using the initial RGB values
        shader.SetUniform4f("u_Colour", colours.R, colours.G, colours.B, 1.0f);
   
        // Bind the vertex array object instead of having to bind the buffer and then add attrib
        vertexArray.Bind();

        // Bind the index buffer
        indexBuffer.Bind();

        // Draw call which uses the index array instead of raw positions 
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    glfwTerminate();
    return 0;
}

