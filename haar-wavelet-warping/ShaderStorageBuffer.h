#pragma once
#include <glad/include/glad/glad.h>
#include <vector>
#include "ReferenceCountedObject.h"

class ShaderStorageBuffer : public ReferenceCountedObject
{
	GLuint m_ID = 0u;
public:
	static std::shared_ptr<ShaderStorageBuffer> create(unsigned int size, void* data, unsigned int index, GLuint usage = GL_DYNAMIC_DRAW)
	{
		return createShared<ShaderStorageBuffer>(size, data, index, usage);
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

protected:
	ShaderStorageBuffer() = default;
	ShaderStorageBuffer(unsigned int size, void* data, unsigned int index, GLuint usage = GL_DYNAMIC_DRAW)
	{
		glGenBuffers(1, &m_ID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
	}
};