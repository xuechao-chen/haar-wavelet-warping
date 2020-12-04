#pragma once
#include <vector>
#include <iostream>

using std::vector;

class HaarSlice
{
	uint16_t m_Width;
	uint16_t m_Height;
	double* m_pData;

public:
	HaarSlice(uint16_t width, uint16_t height, double* data) : m_Width(width), m_Height(height)
	{
		m_pData = new double[width * height];
		for (uint16_t i = 0; i < width*height; i++)
		{
			m_pData[i] = data[i];
		}
	}
	~HaarSlice()
	{
		delete m_pData;
	}

	double get(uint16_t i, uint16_t j) const { return m_pData[i * m_Width + j]; }
	uint16_t width() const { return m_Width; }
	uint16_t height() const { return m_Height; }
};

struct FinerCoeff
{
	double a, b, c, d;
};

class Haar2D
{
	vector<HaarSlice> m_HaarMipmap;

public:
	Haar2D(const HaarSlice& image)
	{
		generateMipmap(image);
	}

	uint16_t getSliceNum() const { return (uint16_t)m_HaarMipmap.size(); }

	HaarSlice getSliceAt(uint16_t l) const { return m_HaarMipmap[l]; }

	FinerCoeff getFinerCoeff(uint16_t l, uint16_t u, uint16_t v) const
	{
		auto a = m_HaarMipmap[l + 1].get(u, v);
		auto b = m_HaarMipmap[l + 1].get(u+1, v);
		auto c = m_HaarMipmap[l + 1].get(u, v+1);
		auto d = m_HaarMipmap[l + 1].get(u+1, v+1);

		return { a,b,c,d };
	}

private:
	void generateMipmap(const HaarSlice& image)
	{
		m_HaarMipmap.push_back(image);

		uint16_t Width = image.width();
		uint16_t Height = image.height();

		do
		{
			HaarSlice LastSlice = m_HaarMipmap.back();
			if (Width > 1) Width = Width >> 1;
			if (Height > 1) Height = Height >> 1;

			double* pData = new double[Width * Height];
			for (uint16_t i = 0; i < Width; i++)
			{
				for (uint16_t j = 0; j < Height; j++)
				{
					pData[i * Width + j] = (
						LastSlice.get(i * 2,     j * 2) +
						LastSlice.get(i * 2 + 1, j * 2) +
						LastSlice.get(i * 2,     j * 2 + 1) +
						LastSlice.get(i * 2 + 1, j * 2 + 1))
						/ 4.0f;
				}
			}
			HaarSlice NextSlice = HaarSlice(Width, Height, pData);
			m_HaarMipmap.push_back(NextSlice);
		} while (Width != 1 && Height != 1);

		std::reverse(m_HaarMipmap.begin(), m_HaarMipmap.end());
	}
};