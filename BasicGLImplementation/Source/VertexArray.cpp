#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_iRendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_iRendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];

		// Enable the vertex attribute Array
		GLCall(glEnableVertexAttribArray(i));

		// Set vertex attribute details 
		// This call links the currently bound vertex buffer (at 0 as per glVertexAttribPointer(0... <-- ) 
		// and attribute in the vertex array object above. The vertexArrayObject can then be bound and used instead of bind buffer and glVertexAttribPointer
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(), (const void*)offset));

		offset += element.count * VertexBufferElement::GetTypeSize(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_iRendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}