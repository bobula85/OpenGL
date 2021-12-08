#pragma once

#include <string>

struct shaderProgSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader 
{

public:
	Shader(const std::string& filePath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform4f(const std::string& uniformName, float v1, float v2, float v3, float v4);

private:
	shaderProgSource ParseShader(const std::string& path);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int CompileShader(unsigned int type, const std::string& src);

	int GetUniformLocation(const std::string& uniformName);

	std::string m_FilePath;
	unsigned int m_RendererID;

	// uniform caching
	



};