#pragma once

#include "GL/glew.h"

#include <vector>

#include "Renderer.h"


struct VertexBufferElement
{
	unsigned int	type;
	unsigned int	count;
	unsigned char	normalised;

	static unsigned int GetTypeSize(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:		return 4;
		case GL_INT:		return 4;
		case GL_BYTE:		return 1;
		}

		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:

	unsigned int m_iStride;

	std::vector<VertexBufferElement> m_vElements;

public:

	VertexBufferLayout()
		:m_iStride(0) {}

	~VertexBufferLayout();

	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_vElements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_iStride += count * VertexBufferElement::GetTypeSize(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_vElements.push_back({ GL_UNSIGNED_INT, count, GL_TRUE });
		m_iStride += count * VertexBufferElement::GetTypeSize(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_vElements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE });
		m_iStride += count * VertexBufferElement::GetTypeSize(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_vElements; }
	inline unsigned int GetStride() const { return m_iStride; }
};