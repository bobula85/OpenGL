
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x) GLClearError();\
                  x;\
                  ASSERT(GLLogCall(#x, __FILE__, __LINE__ ))

// Use glGetError to clear all existing errors
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// Use glGetError to get current errors
static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR] - " << error << "'Function name : " << function << " File name : " << file << " Line number : " << line << std::endl;
        return false;
    }
    return true;
}

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

    GLenum err = glewInit();

    if (err != GLEW_OK)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "GLEW Init() Error: %s\n", glewGetErrorString(err);
        // fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        // ...
    }

    // Vertex array of positions
    float SimpleSquarePositions[] =         {-0.5f, -0.5f,
                                              0.5f, -0.5f,
                                              0.5f,  0.5f,
                                             -0.5f,  0.5f };

    // Index array of vertices
    unsigned int SimpleSquareIndices[] =   {0,1,2,
                                            2,3,0};

    // Create a buffer id
    unsigned int bufferId;

    // Create a vertex buffer with the provided id (bufferId) and bind it
    GLCall(glGenBuffers(1, &bufferId));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferId));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), SimpleSquarePositions, GL_STATIC_DRAW));

    // Create a buffer id
    unsigned int indexBuffer;

    // Create an index buffer with the provided id (indexBuffer) and bind it
    GLCall(glGenBuffers(1, &indexBuffer));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), SimpleSquareIndices, GL_STATIC_DRAW));


    // Enable the vertex attribute Array
    GLCall(glEnableVertexAttribArray(0));

    // Set vertex attribute details
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    // Get vertex and fragment shader source code from BasicShader.shader
    shaderProgSource source = ParseShader("Res/Shaders/BasicShader.shader");

    //std::cout << "VERTEX SHADER CODE" << std::endl;
    //std::cout << source.vertexSource << std::endl;
    ///std::cout << "FRAGMENT SHADER CODE" << std::endl;
    //std::cout << source.fragmentSource << std::endl;

    // Create the shader program from the shader sources
    unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
    GLCall(glUseProgram(shader));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

       // GLClearError();
        // Draw call which uses the index array instead of raw positions 
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));
        //ASSERT(GLLogCall());


        // Legacy OpenGL for initial GL check! 
        /*
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f( 0.0f,  0.5f);
            glVertex2f( 0.5f, -0.5f);
        glEnd();
        */

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}