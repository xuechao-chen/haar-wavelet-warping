#pragma once

struct Point2D
{
	double x;
	double y;

	Point2D() : Point2D(0,0) {}
	Point2D(double x, double y) : x(x), y(y) {}

	bool operator>(const Point2D& other) const
	{
		return x > other.x && y > other.y;
	}

	bool operator<(const Point2D& other) const
	{
		return x < other.x&& y < other.y;
	}

	Point2D operator+(const Point2D& other) const
	{
		return Point2D(x + other.x, y + other.y);
	}

	Point2D operator*(const Point2D& other) const
	{
		return Point2D(x*other.x, y*other.y);
	}

};

struct Region2D
{
	Point2D startPos;
	Point2D range;

	Region2D() {}
	Region2D(const Point2D& startPos, const Point2D& range) : startPos(startPos), range(range) {}

	double x1() const { return startPos.x; }
	double x2() const { return startPos.x + range.x; }
	double y1() const { return startPos.y; }
	double y2() const { return startPos.y + range.y; }

	bool contain(const Point2D& point) const { return (point > startPos && point < startPos + range); }
};
