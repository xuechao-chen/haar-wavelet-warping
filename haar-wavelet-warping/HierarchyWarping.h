#pragma once
#include "Haar2D.h"
#include "HammersleySampler.h"

class HierarchyWarping
{
public:
	static void warp(std::shared_ptr<Haar2D> haar2D, std::vector<Point2D>& samples)
	{
		auto SliceNum = haar2D->getSliceNum();

		for (uint16_t i = 0; i < SliceNum - 1; i++)
		{
			auto Slice = haar2D->getSliceAt(i);
			
			auto W    = Slice->width();
			auto H    = Slice->height();
			auto InvW = 1.0f / W;
			auto InvH = 1.0f / H;
			
			for (uint16_t u = 0; u < W; u++)
			{
				for (uint16_t v = 0; v < H; v++)
				{
					auto Coeff = haar2D->getFinerCoeff(i, u, v);

					auto RatioY = (Coeff.a + Coeff.b) / (Coeff.a + Coeff.b + Coeff.c + Coeff.d);
					auto RatioX1 = Coeff.a / (Coeff.a + Coeff.b);
					auto RatioX2 = Coeff.c / (Coeff.c + Coeff.d);

					//TODO: Matrix style
					warpY({ {u * InvW, v * InvH}, {InvW, InvH} }, RatioY, samples);
					warpX({ {u * InvW, v * InvH}, {InvW, InvH * 0.5} }, RatioX1, samples);
					warpX({ {u * InvW, v * InvH + InvH * 0.5}, {InvW, InvH * 0.5} }, RatioX2, samples);
				}
			}
		}
	}

protected:
	HierarchyWarping() = default;

private:
	static void warpX(const Region2D& range2d, double ratio, std::vector<Point2D>& samples)
	{
		for (auto& sample : samples)
		{
			if (!range2d.contain(sample)) continue;

			auto x1 = range2d.x1();
			auto x2 = range2d.x2();

			if (sample.x < x1 + (x2-x1)*ratio)
			{
				sample.x = x1 + (sample.x - x1) * (0.5f / ratio);
			}
			else
			{
				sample.x = x2 + (sample.x - x2) * (0.5f / (1 - ratio));
			}
		}
	}

	static void warpY(const Region2D& range2d, double ratio, std::vector<Point2D>& samples)
	{
		for (auto& sample : samples)
		{
			if (!range2d.contain(sample)) continue;

			auto y1 = range2d.y1();
			auto y2 = range2d.y2();

			if (sample.y < y1 + (y2-y1)*ratio)
			{
				sample.y = y1 + (sample.y - y1) * (0.5f / ratio);
			}
			else
			{
				sample.y = y2 + (sample.y - y2) * (0.5f / (1 - ratio));
			}
		}
	}
};
