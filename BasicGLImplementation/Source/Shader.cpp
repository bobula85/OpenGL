
#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "GL/glew.h"

#include "Renderer.h"

Shader::Shader(const std::string& filePath)
	: m_FilePath(filePath), m_RendererID(0)
{
	shaderProgSource source = ParseShader(filePath);
	m_RendererID = CreateShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

shaderProgSource Shader::ParseShader(const std::string& path)
{
	enum class shaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

		std::cout << "Failed to validate shader program, message: " << message << std::endl;

		GLCall(glDeleteShader(vShaderId));
		GLCall(glDeleteShader(fShaderId));
		GLCall(glDeleteProgram(programId));

		return 0;
	}

	GLCall(glDeleteShader(vShaderId));
	GLCall(glDeleteShader(fShaderId));

	return programId;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& src)
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

		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? " vertex " : " fragment ") << "shader, message: " << message << std::endl;

		GLCall(glDeleteShader(shaderId));

		return 0;
	}

	return shaderId;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& uniformName, float v1, float v2, float v3, float v4)
{
	GLCall(glUniform4f(GetUniformLocation(uniformName), v1, v2, v3, v4));
}

int Shader::GetUniformLocation(const std::string& uniformName)
{
	GLCall(int location = glGetUniformLocation(m_RendererID, uniformName.c_str()));

	if (location == -1)
		std::cout << "Uniform " << uniformName << " doesn't exist" << std::endl;
	
	return location;
}
