#pragma once
#include <glad/include/glad/glad.h>
#include <vector>

class ShaderStorageBuffer
{
	GLuint m_ID = 0u;
public:
	ShaderStorageBuffer() = default;

	ShaderStorageBuffer(unsigned int size, void* data, unsigned int index, GLuint usage = GL_DYNAMIC_DRAW)
	{
		glGenBuffers(1, &m_ID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
	}

	~ShaderStorageBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void bind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID);
	}

	void unbind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
};