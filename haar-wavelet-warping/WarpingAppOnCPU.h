#pragma once
#include "GLApp.h"
#include "Haar2D.h"
#include "HierarchyWarping.h"

class WarpingAppOnCPU : public GLApp
{
    std::shared_ptr<Texture2D> m_Tex;
    std::shared_ptr<ShaderProgram> m_pQuadProgram;
    std::shared_ptr<ShaderProgram> m_pPointProgram;
    std::shared_ptr<VertexBuffer> m_pQuadBuffer;
    std::shared_ptr<VertexBuffer> m_pSamplePointBuffer;

    std::string m_ImagePath = "tex1.jpg";

public:
    WarpingAppOnCPU(unsigned int width, unsigned int height, const std::string& name)
        : GLApp(width, height, name) {}

protected:
    void onInit() override
    {
        m_Tex = Texture2D::create(m_ImagePath.c_str());
        m_pQuadProgram  = ShaderProgram::createRenderProgram("quad.vert", "quad.frag");
        m_pPointProgram = ShaderProgram::createRenderProgram("point.vert", "point.frag");
        m_pQuadBuffer        = createQuadBuffer();
        m_pSamplePointBuffer = createSamplePointBuffer();
    }
    void onRender() override
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_pQuadProgram->use();
        m_pQuadBuffer->bind();
        m_Tex->bind(0);
        m_pQuadProgram->setInt("srcTex", 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        m_pPointProgram->use();
        m_pSamplePointBuffer->bind();
        glPointSize(3.0f);
        glDrawArrays(GL_POINTS, 0, 256);
    }

private:
    std::shared_ptr<VertexBuffer> createQuadBuffer()
    {
        float data[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f,  1.0f,
        };

        GLuint PosIndex = glGetAttribLocation(m_pQuadProgram->m_ID, "pos");
        VertexAttr PosAttr(GL_FLOAT, 2, PosIndex);
        VertexLayout layout; layout.attrs = { PosAttr };
        return VertexBuffer::create(sizeof(data), data, layout);
    }

    std::shared_ptr<VertexBuffer> createSamplePointBuffer()
    {
        auto haar2D = Haar2D::create(m_ImagePath.c_str());

        const int SampleNum = 256;

        std::vector<Point2D> samples = HammersleySampler::create(SampleNum);
        HierarchyWarping::warp(haar2D, samples);

        GLuint PosIndex = glGetAttribLocation(m_pQuadProgram->m_ID, "pos");
        VertexAttr PosAttr(GL_FLOAT, 2, PosIndex);
        VertexLayout layout; layout.attrs = { PosAttr };

        float data[SampleNum * 2];
        for (size_t i = 0; i < SampleNum; i++)
        {
            data[i * 2 + 0] = (samples[i].x - 0.5f) * 2;
            data[i * 2 + 1] = (samples[i].y - 0.5f) * 2;
        }

        return VertexBuffer::create(sizeof(data), data, layout);
    }
};
