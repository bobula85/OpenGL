
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

static unsigned int CompileShader(unsigned int type, const std::string& src)
{
    // Create the shader id 
    unsigned int shaderId = glCreateShader(type);
    
    // convert the source string to a const char pointer because that is what glShaderSource needs
    const char* source = src.c_str();

    // Set the sharder source code using the provided const char pointer 
    glShaderSource(shaderId, 1, &source, nullptr);

    // Compile the shader scource code
    glCompileShader(shaderId);

    // Create int to hold shader query return value
    int shader_compiled;

    // Get shader value based on flag sent as arg (for this cal was GL_COMPILE_STATUS)
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shader_compiled);

    // if the compilation status is not GL_TRUE (if the compile failed)
    if (shader_compiled != GL_TRUE)
    {
        // Get the message from the shader log
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(shaderId, 1024, &log_length, message);
        
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? " vertex " : " fragment ") << "shader, messag: " << message << std::endl;

        glDeleteShader(shaderId);

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
    glAttachShader(programId, vShaderId);
    glAttachShader(programId, fShaderId);

    // Link the program (ie link the vertex and fragment shaders)
    glLinkProgram(programId);

    // Validate the program object (ie is it possible to execute the program)
    glValidateProgram(programId);

    // Create int to hold program query return value
    int program_compiled;

    // Get shader value based on flag sent as arg (for this cal was GL_COMPILE_STATUS)
    glGetProgramiv(programId, GL_VALIDATE_STATUS, &program_compiled);

    // if the compilation status is not GL_TRUE (if the compile failed)
    if (program_compiled != GL_TRUE)
    {
        // Get the message from the shader log
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(programId, 1024, &log_length, message);
        
        std::cout << "Failed to validate shader program, messag: " << message << std::endl;

        glDeleteShader(vShaderId);
        glDeleteShader(fShaderId); 
        glDeleteProgram(programId);

        return 0;
    }

    glDeleteShader(vShaderId);
    glDeleteShader(fShaderId);

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

    GLenum err = glewInit();

    if (err != GLEW_OK)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "GLEW Init() Error: %s\n", glewGetErrorString(err);
        // fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        // ...
    }

    float SimpleTrianglePositions[6] = { -0.5f, -0.5f,
                                          0.0f,  0.5f,
                                          0.5f, -0.5f };

    // Create a buffer id
    unsigned int bufferId;

    // Crete the buffer with the provided id (bufferId)
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), SimpleTrianglePositions, GL_STATIC_DRAW);

    // Enable the vertex attribute Array
    glEnableVertexAttribArray(0);

    // Set vertex attribute details
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    std::string VertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    std::string FragmentShader = 
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 colour;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   colour = vec4(0.0, 1.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(VertexShader, FragmentShader);

    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Legacy OpenGL for initial GL check! 
        /*
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f( 0.0f,  0.5f);
            glVertex2f( 0.5f, -0.5f);
        glEnd();
        */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}