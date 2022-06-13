#pragma once

class Point
{
public:
	int x, y;
	Point();
	Point(int x, int y);
	Point& operator+=(Point&& other);
	Point& operator+=(const Point& other);
	Point& operator-=(Point&& other);
	Point& operator-=(const Point& other);
	Point operator+(const Point& other) const;
	Point operator-(const Point& other) const;
	Point operator-() const;
	Point operator*(int multiplier) const;
	bool operator==(const Point& other) const;
};

