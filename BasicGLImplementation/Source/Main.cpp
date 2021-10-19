
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct shaderProgSource
{
    std::string vertexSource;
    std::string fragmentSource;
};

static shaderProgSource ParseShader(const std::string& path)
{
    enum class shaderType
    {
        NONE      = -1,
        VERTEX    = 0,
        FRAGMENT  = 1
    };

    std::ifstream stream(path);
    std::string line;

    std::stringstream streams[2];

    shaderType type = shaderType::NONE;

    while (std::getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = shaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = shaderType::FRAGMENT;
        }
        else
        {
            streams[(int)type] << line << '\n';
        }
    }

    return { streams[0].str(), streams[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& src)
{
    // Create the shader id 
    unsigned int shaderId = glCreateShader(type);
    
    // convert the source string to a const char pointer because that is what glShaderSource needs
    const char* source = src.c_str();

    // Set the sharder source code using the provided const char pointer 
    GLCall(glShaderSource(shaderId, 1, &source, nullptr));

    // Compile the shader scource code
    GLCall(glCompileShader(shaderId));

    // Create int to hold shader query return value
    int shader_compiled;

    // Get shader value based on flag sent as arg (for this cal was GL_COMPILE_STATUS)
    GLCall(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shader_compiled));

    // if the compilation status is not GL_TRUE (if the compile failed)
    if (shader_compiled != GL_TRUE)
    {
        // Get the message from the shader log
        GLsizei log_length = 0;
        GLchar message[1024];
        GLCall(glGetShaderInfoLog(shaderId, 1024, &log_length, message));
        
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? " vertex " : " fragment ") << "shader, messag: " << message << std::endl;

        GLCall(glDeleteShader(shaderId));

        return 0;
    }

    return shaderId;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // Create the program object that will house the vertex/fragment shaders
    unsigned int programId = glCreateProgram();

    // Use CompileShader function to create the vertex/fragment shaders
    unsigned int vShaderId = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fShaderId = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Attach the shaders to the program object
    GLCall(glAttachShader(programId, vShaderId));
    GLCall(glAttachShader(programId, fShaderId));

    // Link the program (ie link the vertex and fragment shaders)
    GLCall(glLinkProgram(programId));

    // Validate the program object (ie is it possible to execute the program)
    GLCall(glValidateProgram(programId));

    // Create int to hold program query return value
    int program_compiled;

    // Get shader value based on flag sent as arg (for this cal was GL_COMPILE_STATUS)
    GLCall(glGetProgramiv(programId, GL_VALIDATE_STATUS, &program_compiled));

    // if the compilation status is not GL_TRUE (if the compile failed)
    if (program_compiled != GL_TRUE)
    {
        // Get the message from the shader log
        GLsizei log_length = 0;
        GLchar message[1024];
        GLCall(glGetShaderInfoLog(programId, 1024, &log_length, message));
        
        std::cout << "Failed to validate shader program, messag: " << message << std::endl;

        GLCall(glDeleteShader(vShaderId));
        GLCall(glDeleteShader(fShaderId));
        GLCall(glDeleteProgram(programId));

        return 0;
    }

    GLCall(glDeleteShader(vShaderId));
    GLCall(glDeleteShader(fShaderId));

    return programId;
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
    unsigned int vertexArrayObject;
    GLCall(glGenVertexArrays(1, &vertexArrayObject));
    GLCall(glBindVertexArray(vertexArrayObject));

    // Construct VertexBuffer object 
    VertexBuffer vertexBuffer(SimpleSquarePositions, 4 * 2 * sizeof(float));


    // Enable the vertex attribute Array
    GLCall(glEnableVertexAttribArray(0));

    // Set vertex attribute details 
    // This call links the currently bound vertex buffer (at 0 as per glVertexAttribPointer(0... <-- ) 
    // and attribute in the vertex array object above. The vertexArrayObject can then be bound and used instead of bind buffer and glVertexAttribPointer
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    IndexBuffer indexBuffer(SimpleSquareIndices, 6);

    // Get vertex and fragment shader source code from BasicShader.shader
    shaderProgSource source = ParseShader("Res/Shaders/BasicShader.shader");

    // Create the shader program from the shader sources
    unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
    GLCall(glUseProgram(shader));

    // Get the uniform from the shader using it's name
    int location = glGetUniformLocation(shader, "u_Colour");

    // break if the uniform does not exist
    ASSERT(location != -1)

    // set the uniform value useing the correct glUniform** call
    GLCall(glUniform4f(location, 0.0f, 1.0f, 0.0f, 1.0f));

    // Unbind all buffers/programs/attribs by passing 0
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    // Base RGB values
    float rValue = 0.1f;
    float gValue = 0.8f;
    float bValue = 1.0f;

    // Incrementation amounts used to modify the RBG values
    float rIncrement = 0.01f;
    float gIncrement = 0.002f;
    float bIncrement = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        // Bind the shader program
        GLCall(glUseProgram(shader));

        // Set the colour uniform using the initial RGB values
        GLCall(glUniform4f(location, rValue, gValue, bValue, 1.0f));

        // Bind the vertex array object instead of having to bind the buffer and then add attrib
        GLCall(glBindVertexArray(vertexArrayObject));

        // Bind the index buffer
        indexBuffer.Bind();

        // Draw call which uses the index array instead of raw positions 
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        // Check if the RGB values are greater than 1 and modify the incrementation value accordingly
        if (rValue > 1.0f)
        {
            rIncrement = -0.01f;
        }
        else if(rValue < 0.0f)
        {
            rIncrement = 0.01f;
        }

        if (gValue > 1.0f)
        {
            gIncrement = -0.01f;
        }
        else if (gValue < 0.0f)
        {
            gIncrement = 0.01f;
        }

        if (bValue > 1.0f)
        {
            bIncrement = -0.01f;
        }
        else if (bValue < 0.0f)
        {
            bIncrement = 0.01f;
        }

        // Increment the RGB values
        rValue += rIncrement;
        gValue += gIncrement;
        bValue += bIncrement;

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}