#pragma once
#include "GLApp.h"
#include "HammersleySampler.h"

class WarpingAppOnGPUByLayer : public GLApp
{
	unsigned int m_NumSample = 256;
	unsigned int m_NumProbe = 256;
	std::shared_ptr<ShaderProgram> m_pQuadProgram;
	std::shared_ptr<ShaderProgram> m_pInitWarpingProgram;
	std::shared_ptr<ShaderProgram> m_pWarpingProgram;
	std::shared_ptr<ShaderProgram> m_pVisualizeWarpingProgram;
	std::shared_ptr<Texture2DArray> m_pTexArray;
	std::shared_ptr<VertexBuffer> m_pQuadBuffer;
	std::shared_ptr<ShaderStorageBuffer> m_pUniformSampleBuffer;
	std::shared_ptr<ShaderStorageBuffer> m_pWarpingSampleBuffer;
	std::shared_ptr<Image2D> m_pImage2D;
public:
	WarpingAppOnGPUByLayer(unsigned int width, unsigned int height, const std::string& name) : GLApp(width, height, name){}

protected:
	void onInit() override
	{
		m_pQuadProgram = ShaderProgram::create("quad_tex2d_array.vert", "quad_tex2d_array.frag");
		m_pInitWarpingProgram = ShaderProgram::create("init_warping_point.comp");
		m_pWarpingProgram = ShaderProgram::create("warping_on_gpu_by_layer.comp");
		m_pVisualizeWarpingProgram = ShaderProgram::create("visualize_warping_point.comp");

		std::vector<const char*> PathArray;
		for (size_t i = 0; i < 256; i++)
		{
			PathArray.push_back("tex4.jpg");
		}
		m_pTexArray = Texture2DArray::create(PathArray, 16, 16);
		m_pQuadBuffer = createQuadBuffer();
		m_pUniformSampleBuffer = createUniformSampleBuffer(m_NumSample, 0);
		m_pWarpingSampleBuffer = ShaderStorageBuffer::create(sizeof(float) * m_NumSample * m_NumProbe * 2, nullptr, 1);
		m_pImage2D = Image2D::createEmpty(256, 256, GL_R32F, GL_RED, 0);
	}

	void onRender() override
	{
		glClearColor(1.0,1.0,1.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		m_pInitWarpingProgram->use();
		glDispatchCompute(256,1,1);

		m_pWarpingProgram->use();
		m_pTexArray->bind(0);
		auto TotalLayer = std::log2(m_pTexArray->width())+1;
		m_pWarpingProgram->setInt("totalLayer", TotalLayer);
		for (size_t i = 0; i < TotalLayer-1; i++)
		{
			m_pWarpingProgram->setInt("curLayer", i);
			auto TexSize = 1 << i;
			glDispatchCompute(m_NumProbe, TexSize, TexSize);
			glMemoryBarrier(GL_SHADER_STORAGE_BUFFER);
		}

		m_pVisualizeWarpingProgram->use();
		glDispatchCompute(16,16,1);

		m_pQuadProgram->use();
		m_pQuadBuffer->bind();
		m_pTexArray->bind(0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

private:
	std::shared_ptr<ShaderStorageBuffer> createUniformSampleBuffer(unsigned int numSample, unsigned int bindPoint)
	{
		auto UniformSample = HammersleySampler::create(numSample);
		float* pData = new float[numSample *2];
		for (size_t i = 0; i < numSample; i++)
		{
			pData[i * 2 + 0] = UniformSample[i].x;
			pData[i * 2 + 1] = UniformSample[i].y;
		}
		return ShaderStorageBuffer::create(sizeof(float)*numSample*2, pData, bindPoint, GL_STATIC_COPY);
	}

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
};