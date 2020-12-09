#pragma once
#include <vector>
#include "ReferenceCountedObject.h"
#include "stb_image.h"

struct FinerCoeff
{
    /********
    * a | b *
    * c | d *
    *********/
    double a, b, c, d;
};

class HaarSlice : public ReferenceCountedObject
{
	uint16_t m_Width;
	uint16_t m_Height;
	double* m_pData;

public:
	static std::shared_ptr<HaarSlice> create(uint16_t width, uint16_t height, double* data)
	{
		return createShared<HaarSlice>(width, height, data);
	}

	~HaarSlice()
	{
		delete m_pData;
	}

	double get(uint16_t i, uint16_t j) const 
	{
		//warp mode: clamp to border
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		if (i >= m_Width)  i = m_Width - 1;
		if (j >= m_Height) j = m_Height - 1;
		
		return m_pData[j * m_Width + i]; 
	}
	uint16_t width() const { return m_Width; }
	uint16_t height() const { return m_Height; }

protected:
	HaarSlice(uint16_t width, uint16_t height, double* data) : m_Width(width), m_Height(height)
	{
		int Size = width * height;
		m_pData = new double[Size];
		for (int i = 0; i < Size; i++)
		{
			m_pData[i] = data[i];
		}
	}
};

class Haar2D : public ReferenceCountedObject
{
	std::vector<std::shared_ptr<HaarSlice>> m_pHaarMipmap;

public:
	static std::shared_ptr<Haar2D> create(const char* imagePath)
	{
		return createShared<Haar2D>(imagePath);
	}

	uint16_t getSliceNum() const { return (uint16_t)m_pHaarMipmap.size(); }

	std::shared_ptr<HaarSlice> getSliceAt(uint16_t l) const { return m_pHaarMipmap[l]; }

	FinerCoeff getFinerCoeff(uint16_t l, uint16_t u, uint16_t v) const
	{
		auto a = m_pHaarMipmap[l+1]->get(u*2,  v*2);
		auto b = m_pHaarMipmap[l+1]->get(u*2+1,v*2);
		auto c = m_pHaarMipmap[l+1]->get(u*2,  v*2+1);
		auto d = m_pHaarMipmap[l+1]->get(u*2+1,v*2+1);

		return { a,b,c,d };
	}

protected:
	Haar2D(const char* imagePath)
	{
		int Width, Height, Channels;
		unsigned char* pImageData = stbi_load(imagePath, &Width, &Height, &Channels, 0);
		double* pSliceData = new double[Width * Height];

		for (int i = 0; i < Width * Height; i++)
		{
			switch (Channels)
			{
			case 1:
			{
				pSliceData[i] = pImageData[i];
				break;
			}
			case 3:
			{
				pSliceData[i] = 0.3 * pImageData[i * 3 + 0] + 0.59 * pImageData[i * 3 + 1] + 0.11 * pImageData[i * 3 + 2];
				break;
			}
			default:
			{
				std::cout << "Only support image format(R, RGB)" << std::endl;
				exit(-1);
				break;
			}
			}
		}

		auto pSlice = HaarSlice::create(Width, Height, pSliceData);
		generateMipmap(pSlice);
		stbi_image_free(pImageData);
	}

private:
	void generateMipmap(std::shared_ptr<HaarSlice> finestSlice)
	{
		m_pHaarMipmap.push_back(finestSlice);

		auto Width = finestSlice->width();
		auto Height = finestSlice->height();

		do
		{
			auto LastSlice = m_pHaarMipmap.back();
			if (Width > 1) Width = Width >> 1;
			if (Height > 1) Height = Height >> 1;

			double* pSliceData = new double[Width * Height];
			for (auto i = 0; i < Width; i++)
			{
				for (auto j = 0; j < Height; j++)
				{
					pSliceData[j * Width + i] = (
						LastSlice->get(i * 2, j * 2) +
						LastSlice->get(i * 2 + 1, j * 2) +
						LastSlice->get(i * 2, j * 2 + 1) +
						LastSlice->get(i * 2 + 1, j * 2 + 1))
						/ 4.0f;
				}
			}
			auto NextSlice = HaarSlice::create(Width, Height, pSliceData);
			m_pHaarMipmap.emplace_back(NextSlice);
		} while (Width != 1 && Height != 1);

		std::reverse(m_pHaarMipmap.begin(), m_pHaarMipmap.end());
	}
};