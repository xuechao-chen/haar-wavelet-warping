#pragma once
#include "Haar2D.h"
#include "HammersleySampler.h"

struct WarpingMatrix
{
	double m00, m01=0, m02, m10=0, m11, m12, m20=0, m21=0, m22=1;

	WarpingMatrix() {}
	WarpingMatrix(double baseX, double baseY, double ratioX, double ratioY)
	{
		m00 = 0.5 / ratioX;
		m02 = (1 - m00) * baseX;
		m11 = 0.5 / ratioY;
		m12 = (1 - m11) * ratioY;
	}

	WarpingMatrix& operator*(const WarpingMatrix& other) const
	{
		WarpingMatrix Matrix;

		Matrix.m00 = m00 * other.m00 + m01 * other.m01 + m02 * other.m02;
		Matrix.m01 = m00 * other.m10 + m01 * other.m11 + m02 * other.m12;
		Matrix.m02 = m00 * other.m20 + m01 * other.m21 + m02 * other.m22;

		Matrix.m10 = m10 * other.m00 + m11 * other.m01 + m12 * other.m02;
		Matrix.m11 = m10 * other.m10 + m11 * other.m11 + m12 * other.m12;
		Matrix.m12 = m10 * other.m20 + m11 * other.m21 + m12 * other.m22;

		return Matrix;
	}

	Point2D& operator*(const Point2D& other) const
	{
		Point2D Point;

		auto x = Point.x;
		auto y = Point.y;
		Point.x = m00 * x + m01 * y + m02;
		Point.y = m10 * x + m11 * y + m21;

		return Point;
	}
};

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

private:
	static void warpX(const Region2D& region2d, double ratio, std::vector<Point2D>& samples)
	{
		for (auto& sample : samples)
		{
			if (!region2d.contain(sample)) continue;

			auto x1 = region2d.x1();
			auto x2 = region2d.x2();

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

	static void warpY(const Region2D& region2d, double ratio, std::vector<Point2D>& samples)
	{
		for (auto& sample : samples)
		{
			if (!region2d.contain(sample)) continue;

			auto y1 = region2d.y1();
			auto y2 = region2d.y2();

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
