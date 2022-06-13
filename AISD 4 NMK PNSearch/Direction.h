#pragma once
#include "Point.h"

class Direction
{
public:
	enum Directions
	{
		N = 0,
		NE,
		E,
		SE,
		S,
		SW,
		W,
		NW,
		DIRECTIONS_AMOUNT,
	};
private:	
	Directions dir;

public:
	Direction();
	Direction(Directions d);
	Direction& setDir(Directions d);
	bool operator==(Direction& other) const;
	Direction& operator++();
	Point getP() const;
	int getDX() const;
	int getDY() const;
	Direction& operator=(Directions d);
	bool operator<(Directions d) const;
};