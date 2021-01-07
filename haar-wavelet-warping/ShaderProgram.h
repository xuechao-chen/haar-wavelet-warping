#pragma once
#include <glad/include/glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ReferenceCountedObject.h"

class Shader : public ReferenceCountedObject
{
public:
    unsigned int m_ID;
    GLuint m_Type;

    static std::shared_ptr<Shader> create(const char* path, GLuint type)
    {
        return createShared<Shader>(path, type);
    }

    ~Shader()
    {
        glDeleteShader(m_ID);
    }

protected:
    Shader(const char* path, GLuint type) : m_Type(type)
    {
        std::string Code;
        readShader(path, Code);

        const char* ShaderCode = Code.c_str();
        m_ID = glCreateShader(type);
        glShaderSource(m_ID, 1, &ShaderCode, NULL);
        glCompileShader(m_ID);
        checkCompileErrors(type);
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
    bool readText(std::string path, std::string& out)
    {
        std::ifstream ShaderFile;
        ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            ShaderFile.open(path);
            std::stringstream ShaderStream;
            ShaderStream << ShaderFile.rdbuf();
            ShaderFile.close();
            out = ShaderStream.str();
            return true;
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << "\n" << e.what() << std::endl;
            return false;
        }
    }
    //https://diharaw.github.io/blog/dead-simple-shader-include-system/
    bool readShader(std::string path, std::string& out)
    {
        std::string og_source;

        if (!readText(path, og_source))
            return false;

        std::istringstream stream(og_source);
        std::string line;

        while (std::getline(stream, line))
        {
            if (line.find("#include") != std::string::npos)
            {
                size_t start = line.find_first_of("<") + 1;
                size_t end = line.find_last_of(">");
                std::string include_path = line.substr(start, end - start);
                std::string path_to_shader = "";
                size_t slash_pos = path.find_last_of("/");

                if (slash_pos != std::string::npos) path_to_shader = path.substr(0, slash_pos + 1);

                std::string include_source;
                if (!readShader(path_to_shader + include_path, include_source))
                {
                    std::cout << "Included file <" << include_path << "> cannot be opened!" << std::endl;
                    return false;
                }
                else
                {
                    out += include_source + "\n\n";
                }
            }
            else
            {
                out += line + "\n";
            }
        }

        return true;
    }
};

class ShaderProgram : public ReferenceCountedObject
{
public:
    unsigned int m_ID;

    static std::shared_ptr<ShaderProgram> create(const char* vertexPath, const char* fragmentPath)
    {
        return createShared<ShaderProgram>(vertexPath, fragmentPath);
    }

    static std::shared_ptr<ShaderProgram> create(const char* computePath)
    {
        return createShared<ShaderProgram>(computePath);
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

    void setVec2(const std::string& name, float v1, float v2) const
    {
        glUniform2f(glGetUniformLocation(m_ID, name.c_str()), v1, v2);
    }

protected:
    ShaderProgram() = default;
    ShaderProgram(const char* vertexPath, const char* fragmentPath)
    {
        m_ID = glCreateProgram();
        auto VertexShader = Shader::create(vertexPath, GL_VERTEX_SHADER);
        auto FragmentShader = Shader::create(fragmentPath, GL_FRAGMENT_SHADER);
        glAttachShader(m_ID, VertexShader->m_ID);
        glAttachShader(m_ID, FragmentShader->m_ID);
        glLinkProgram(m_ID);
        checkLinkErrors();
    }

    ShaderProgram(const char* computePath)
    {
        m_ID = glCreateProgram();
        auto ComputeShader = Shader::create(computePath, GL_COMPUTE_SHADER);
        glAttachShader(m_ID, ComputeShader->m_ID);
        glLinkProgram(m_ID);
        checkLinkErrors();
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