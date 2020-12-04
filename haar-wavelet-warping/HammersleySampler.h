#pragma once
#include <vector>
#include "Common.h"

class HammersleySampler
{
public:
	static std::vector<Point2D> create(uint32_t num)
	{
		std::vector<Point2D> Samples;
		for (uint32_t i = 0; i < num; i++)
		{
			Samples.push_back(__hammersley2d(i, num));
		}
		return Samples;
	}

private:
	HammersleySampler() = default;

private:
	static double __radicalInverse(uint32_t bits)
	{
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return double(bits) * 2.3283064365386963e-10; // / 0x100000000
	}

	static Point2D __hammersley2d(uint32_t i, uint32_t N)
	{
		return Point2D(double(i) / double(N), __radicalInverse(i));
	}
};
