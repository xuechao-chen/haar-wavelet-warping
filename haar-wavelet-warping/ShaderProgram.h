#pragma once

#include <glad/include/glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int m_ID;
    GLuint m_Type;

    Shader(const char* path, GLuint type) : m_Type(type)
    {
        std::string Code;
        std::ifstream ShaderFile;
        ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            ShaderFile.open(path);
            std::stringstream ShaderStream;
            ShaderStream << ShaderFile.rdbuf();
            ShaderFile.close();
            Code = ShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << "\n" << e.what() << std::endl;
        }

        const char* ShaderCode = Code.c_str();
        m_ID = glCreateShader(type);
        glShaderSource(m_ID, 1, &ShaderCode, NULL);
        glCompileShader(m_ID);
        checkCompileErrors(type);
    }

    ~Shader()
    {
        glDeleteShader(m_ID);
    }

private:
    void checkCompileErrors(GLuint shaderType)
    {
        int success;
        char infoLog[1024];

        glGetShaderiv(m_ID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(m_ID, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << shaderType << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
};

class ShaderProgram
{
public:
    unsigned int m_ID;

    static std::shared_ptr<ShaderProgram> createRenderProgram(const char* vertexPath, const char* fragmentPath)
    {
        return std::make_shared<ShaderProgram>(vertexPath, fragmentPath);
    }

    static std::shared_ptr<ShaderProgram> createComputeProgram(const char* computePath)
    {
        return std::make_shared<ShaderProgram>(computePath);
    }

    ShaderProgram() = default;
    ShaderProgram(const char* vertexPath, const char* fragmentPath)
    {
        m_ID = glCreateProgram();
        Shader vertexShader(vertexPath, GL_VERTEX_SHADER);
        Shader fragmentShader(fragmentPath, GL_FRAGMENT_SHADER);
        glAttachShader(m_ID, vertexShader.m_ID);
        glAttachShader(m_ID, fragmentShader.m_ID);
        glLinkProgram(m_ID);
        checkLinkErrors();
    }

    ShaderProgram(const char* computePath)
    {
        m_ID = glCreateProgram();
        Shader computeShader(computePath, GL_COMPUTE_SHADER);
        glAttachShader(m_ID, computeShader.m_ID);
        glLinkProgram(m_ID);
        checkLinkErrors();
    }

    void use()
    {
        glUseProgram(m_ID);
    }

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
    }

    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
    }
    
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
    }

private:
    void checkLinkErrors()
    {
        int success;
        char infoLog[1024];

        glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_ID, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR: " << "\n" << infoLog <<
                "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
};