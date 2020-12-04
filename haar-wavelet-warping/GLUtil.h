#pragma once
#include <glad/include/glad/glad.h>
#include "HammersleySampler.h"

class GLUtil
{
public:
    static GLuint genTexture() {
        // We create a single float channel 512^2 texture
        GLuint texHandle;
        glGenTextures(1, &texHandle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 512, 512, 0, GL_RED, GL_FLOAT, NULL);

        // Because we're also using this tex as an image (in order to write to it),
        // we bind it to an image unit as well
        glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        return texHandle;
    }

    static GLuint genBuffer()
    {
        auto SamplePoints = HammersleySampler::create(256);
        float x[256];
        for (size_t i = 0; i < SamplePoints.size(); i++)
        {
            x[i] = (float)SamplePoints[i].x;
        }

        GLuint BufferID;
        glGenBuffers(1, &BufferID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, BufferID);

       // glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(x), x, GL_DYNAMIC_DRAW);
        return BufferID;
    }
};