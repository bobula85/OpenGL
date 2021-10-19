#pragma once
#pragma once

class IndexBuffer
{
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void UnBind() const;

	inline unsigned int GetCount() { return m_Count; }

private:

	unsigned int m_RendererId;
	unsigned int m_Count;
};