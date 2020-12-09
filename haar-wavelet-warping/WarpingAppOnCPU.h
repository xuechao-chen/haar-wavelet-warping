#pragma once
#include "GLApp.h"
#include "Haar2D.h"
#include "HierarchyWarping.h"

class WarpingAppOnCPU : public GLApp
{
    std::shared_ptr<Texture2D> m_pTex;
    std::shared_ptr<ShaderProgram> m_pQuadProgram;
    std::shared_ptr<ShaderProgram> m_pPointProgram;
    std::shared_ptr<VertexBuffer> m_pQuadBuffer;
    std::shared_ptr<VertexBuffer> m_pSamplePointBuffer;

    std::string m_ImagePath = "tex2.jpg";
    unsigned int m_SampleNum = 64;

public:
    WarpingAppOnCPU(unsigned int width, unsigned int height, const std::string& name)
        : GLApp(width, height, name) {}

protected:
    void onInit() override
    {
        m_pTex = Texture2D::create(m_ImagePath.c_str());
        m_pQuadProgram  = ShaderProgram::create("quad.vert", "quad.frag");
        m_pPointProgram = ShaderProgram::create("point.vert", "point.frag");
        m_pQuadBuffer        = createQuadBuffer();
        m_pSamplePointBuffer = createSamplePointBuffer();
    }

    void onRender() override
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_pQuadProgram->use();
        m_pQuadBuffer->bind();
        m_pTex->bind(0);
        m_pQuadProgram->setInt("srcTex", 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        m_pPointProgram->use();
        m_pSamplePointBuffer->bind();
        glPointSize(3.0f);
        glDrawArrays(GL_POINTS, 0, m_SampleNum);
    }

private:
    std::shared_ptr<VertexBuffer> createQuadBuffer()
    {
        float Data[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f,
        };

        GLuint PosIndex = glGetAttribLocation(m_pQuadProgram->m_ID, "pos");
        VertexAttr PosAttr(GL_FLOAT, 2, PosIndex);
        VertexLayout Layout; Layout.attrs = { PosAttr };
        return VertexBuffer::create(sizeof(Data), Data, Layout);
    }

    std::shared_ptr<VertexBuffer> createSamplePointBuffer()
    {
        auto Haar2D = Haar2D::create(m_ImagePath.c_str());

        std::vector<Point2D> Samples = HammersleySampler::create(m_SampleNum);
        HierarchyWarping::warp(Haar2D, Samples);

        GLuint PosIndex = glGetAttribLocation(m_pQuadProgram->m_ID, "pos");
        VertexAttr PosAttr(GL_FLOAT, 2, PosIndex);
        VertexLayout Layout; Layout.attrs = { PosAttr };

        float* SamplePointData = new float[m_SampleNum*2];
        for (size_t i = 0; i < m_SampleNum; i++)
        {
            SamplePointData[i * 2 + 0] = (Samples[i].x - 0.5f) * 2;
            SamplePointData[i * 2 + 1] = (Samples[i].y - 0.5f) * 2;
        }

        auto SamplePointBuffer =  VertexBuffer::create(sizeof(float)*m_SampleNum*2, SamplePointData, Layout);
        delete[] SamplePointData;
        return SamplePointBuffer;
    }
};
