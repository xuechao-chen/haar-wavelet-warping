#pragma once
#include "GLApp.h"
#include "HammersleySampler.h"

class SimpleApp : public GLApp
{
    unsigned int m_Frame = 0;

    std::shared_ptr<ShaderProgram> m_pQuadProgram;
    std::shared_ptr<ShaderProgram> m_pCompProgram;
    std::shared_ptr<ShaderStorageBuffer> m_pSSBO;
    std::shared_ptr<VertexBuffer> m_pVertexBuffer;
    std::shared_ptr<Image2D> m_pDestTex;

public:
    SimpleApp(unsigned int width, unsigned int height, const std::string& name)
        : GLApp(width, height, name) {}

protected:
    void onInit() override
    {
        m_pDestTex = Image2D::createEmpty(512, 512, GL_R32F, GL_RED, 0);

        auto SamplePoints = HammersleySampler::create(256);
        float x[256];
        for (size_t i = 0; i < SamplePoints.size(); i++)
        {
            x[i] = (float)SamplePoints[i].x;
        }

        m_pQuadProgram = std::make_shared<ShaderProgram>("quad.vert", "quad.frag");
        m_pCompProgram = std::make_shared<ShaderProgram>("warping.comp");

        m_pSSBO = ShaderStorageBuffer::create(sizeof(x), x, 1);

        float data[] = {
            -1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, -1.0f,
            1.0f, 1.0f
        };

        GLuint PosIndex = glGetAttribLocation(m_pQuadProgram->m_ID, "pos");
        VertexAttr PosAttr(GL_FLOAT, 2, PosIndex);
        VertexLayout layout; layout.attrs = { PosAttr };
        m_pVertexBuffer = VertexBuffer::create(sizeof(data), data, layout);
    }

    void onRender() override
    {
        {
            m_pSSBO->bind();
            m_pCompProgram->use();
            glUniform1i(glGetUniformLocation(m_pCompProgram->m_ID, "destTex"), 0);
            glUniform1i(glGetUniformLocation(m_pCompProgram->m_ID, "SamplePoints"), 1);
            glUniform1f(glGetUniformLocation(m_pCompProgram->m_ID, "roll"), (m_Frame++) % 1024 * 0.01f);
            glDispatchCompute(512 / 16, 512 / 16, 1);
        }

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        {
            m_pVertexBuffer->bind();
            m_pQuadProgram->use();
            glUniform1i(glGetUniformLocation(m_pQuadProgram->m_ID, "srcTex"), 0);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
};