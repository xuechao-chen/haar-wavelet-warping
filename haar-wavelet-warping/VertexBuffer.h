#pragma once
#include <glad/include/glad/glad.h>
#include <vector>

struct VertexAttr
{
	GLuint format; //only support GL_INT, GL_FLOAT
	GLuint size;
	GLuint index;

	VertexAttr(GLuint format, GLuint size, GLuint index) 
		: format(format), size(size), index(index) {}

	GLuint stride() const
	{
		return size * (format == GL_INT ? sizeof(int) : sizeof(float));
	}
};

struct VertexLayout
{
	std::vector<VertexAttr> attrs;
};

class VertexBuffer
{
	GLuint m_VBO = 0u;
	GLuint m_VAO = 0u;
public:
	static std::shared_ptr<VertexBuffer> create(unsigned int size, void* data, const VertexLayout& layout, GLuint usage = GL_STATIC_DRAW)
	{
		return std::make_shared<VertexBuffer>(size, data, layout, usage);
	}
	~VertexBuffer() 
	{
		glDeleteBuffers(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
	}

	void bind() const
	{
		glBindVertexArray(m_VAO);
	}

	void unbind() const
	{
		glBindVertexArray(0);
	}

//should be protected
public:
	VertexBuffer() = default;
	VertexBuffer(unsigned int size, void* data, const VertexLayout & layout, GLuint usage = GL_STATIC_DRAW)
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);

		unsigned int Pointer = 0;
		for (size_t i = 0; i < layout.attrs.size(); i++)
		{
			auto& attr = layout.attrs[i];

			glVertexAttribPointer(attr.index, attr.size, attr.format, GL_FALSE, attr.stride(), (void*)Pointer);
			glEnableVertexAttribArray(attr.index);
			Pointer += attr.stride();
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};