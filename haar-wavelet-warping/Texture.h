#pragma once
#include <glad/include/glad/glad.h>
#include <memory>
#include "stb_image.h"
#include "ReferenceCountedObject.h"

class Texture2D : public ReferenceCountedObject
{
	unsigned int m_ID = 0;
	unsigned int m_BindPoint = 0;
	int m_Width = 0;
	int m_Height = 0;
	GLenum m_Format = GL_RGB;
public:
	static std::shared_ptr<Texture2D> create(unsigned int width, unsigned int height, GLint internalFormat, GLenum format, bool mipmap)
	{
		return createShared<Texture2D>(width, height, internalFormat, format, mipmap);
	}

	static std::shared_ptr<Texture2D> create(const char* path)
	{
		return createShared<Texture2D>(path);
	}

	static std::shared_ptr<Texture2D> create()
	{
		return nullptr;
	}

	void bind(unsigned int bindPoint)
	{
		glActiveTexture(GL_TEXTURE0+bindPoint);
		glBindTexture(GL_TEXTURE_2D, m_ID);
		m_BindPoint = bindPoint;
	}

	void unbind() const
	{
		glActiveTexture(GL_TEXTURE0 + m_BindPoint);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	int    width()  const { return m_Width; }
	int    height() const { return m_Height; }
	GLenum format() const { return m_Format; }

	void getTexImage(std::vector<unsigned char>& texData)
	{
		if (m_Format == GL_RED) texData.resize(m_Width*m_Height);
		if (m_Format == GL_RGB) texData.resize(m_Width*m_Height*3);
		if (m_Format == GL_RGBA) texData.resize(m_Width*m_Height*4);

		glBindTexture(GL_TEXTURE_2D, m_ID);
		glGetTexImage(GL_TEXTURE_2D, 0, m_Format, GL_UNSIGNED_BYTE, &texData[0]);
	}

	~Texture2D() { glDeleteTextures(1, &m_ID); }

protected:
	Texture2D() = default;
	Texture2D(unsigned int width, unsigned int height, GLint internalFormat, GLenum format, bool mipmap)
		: m_Width(width), m_Height(height)
	{
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, nullptr);

		if (mipmap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_AUTO_GENERATE_MIPMAP, GL_TRUE);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	Texture2D(const char* path)
	{
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		int Channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* pImageData = stbi_load(path, &m_Width, &m_Height, &Channels, 0);

		switch (Channels)
		{
		case 1:
			m_Format = GL_RED;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, pImageData);
			break;
		case 3:
			m_Format = GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, pImageData);
			break;
		case 4:
			m_Format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageData);
			break;
		default:
			break;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(pImageData);
	}
};

class Texture2DArray : public ReferenceCountedObject
{
	unsigned int m_ID = 0;
	unsigned int m_BindPoint = 0;
	int m_Width = 0;
	int m_Height = 0;
	int m_Layer = 1;
	GLenum m_Format = GL_RGB;
public:
	static std::shared_ptr<Texture2DArray> create(unsigned int width, unsigned int height, unsigned int layer, GLint internalFormat, GLenum format)
	{
		return createShared<Texture2DArray>(width, height, layer, internalFormat, format);
	}

	static std::shared_ptr<Texture2DArray> create(std::vector<const char*> pathArray, unsigned width, unsigned height)
	{
		return createShared<Texture2DArray>(pathArray, width, height);
	}

	static std::shared_ptr<Texture2DArray> create()
	{
		return nullptr;
	}

	void bind(unsigned int bindPoint)
	{
		glActiveTexture(GL_TEXTURE0 + bindPoint);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);
		m_BindPoint = bindPoint;
	}

	void unbind() const
	{
		glActiveTexture(GL_TEXTURE0 + m_BindPoint);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	int    width()  const { return m_Width; }
	int    height() const { return m_Height; }
	GLenum format() const { return m_Format; }

	~Texture2DArray() { glDeleteTextures(1, &m_ID); }

protected:
	Texture2DArray() = default;
	Texture2DArray(unsigned int width, unsigned int height, unsigned int layer, GLint internalFormat, GLenum format)
		: m_Width(width), m_Height(height), m_Layer(layer)
	{
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat,width, height, layer);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layer, format, GL_UNSIGNED_BYTE, nullptr);

		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
	Texture2DArray(std::vector<const char*> pathArray, unsigned width, unsigned height)
		: m_Width(width), m_Height(height), m_Layer(pathArray.size())
	{
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//TODO: 写死RGB通道格式
		unsigned char* pImageData = new unsigned char[width*height*3*pathArray.size()];
		int pointer = 0;
		for (size_t i = 0; i < pathArray.size(); i++)
		{
			int Channels;
			stbi_set_flip_vertically_on_load(true);
			unsigned char* pData = stbi_load(pathArray[i], &m_Width, &m_Height, &Channels, 0);
			
			memcpy(pImageData+pointer, pData, m_Width*m_Height*3);
			pointer += m_Width * m_Height * 3;

			stbi_image_free(pData);
		}

		int Level = log2(m_Width)+1;
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, Level, GL_RGB16F, width, height, pathArray.size());
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, pathArray.size(), GL_RGB, GL_UNSIGNED_BYTE, pImageData);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		delete[] pImageData;
	}
};

class Image2D : public ReferenceCountedObject
{
	unsigned int m_ID;
public:
	static std::shared_ptr<Image2D> createEmpty(unsigned int width, unsigned int height, GLint internalFormat, GLenum format, unsigned int bindPoint) 
	{
		return createShared<Image2D>(width, height, internalFormat, format, bindPoint);
	}
	~Image2D()
	{
		glDeleteTextures(1, &m_ID);
	}

protected:
	Image2D() = default;
	Image2D(unsigned int width, unsigned int height, GLint internalFormat, GLenum format, unsigned int bindPoint)
	{
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, nullptr);

		glBindImageTexture(bindPoint, m_ID, 0, GL_FALSE, 0, GL_READ_WRITE, internalFormat);
	}
};
