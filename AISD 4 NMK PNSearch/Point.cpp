#include "Point.h"

Point::Point(int x, int y) :
	x(x), y(y)
{}

Point::Point() :
	x(0), y(0)
{}

Point& Point::operator+=(Point&& other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

Point& Point::operator-=(Point&& other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

Point& Point::operator+=(const Point& other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

Point& Point::operator-=(const Point& other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

Point Point::operator+(const Point& other) const
{
	return Point(this->x + other.x, this->y + other.y);
}

Point Point::operator-(const Point& other) const
{
	return Point(this->x - other.x, this->y - other.y);
}

Point Point::operator-() const 
{
	return *this * -1;
}

Point Point::operator*(int multiplier) const 
{
	return Point(this->x * multiplier, this->y * multiplier);
}

bool Point::operator==(const Point& other) const
{
	return this->x == other.x && this->y == other.y;
}