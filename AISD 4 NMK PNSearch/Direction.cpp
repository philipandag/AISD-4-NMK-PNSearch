#include "Direction.h"

Direction::Direction()
{
	setDir(N);
}

Direction::Direction(Directions d)
{
	setDir(d);
}

Direction& Direction::setDir(Directions d)
{
	this->dir = d;
	return *this;
}

bool Direction::operator==(Direction& other) const
{
	return this->dir == other.dir;
}

Direction& Direction::operator++()
{
	this->dir = static_cast<Directions>((this->dir + 1) % DIRECTIONS_AMOUNT);
	return *this;
}

int Direction::getDX() const
{
	return dir > N && dir < S ? 1 
		: dir > S || dir < N ? -1
		: 0;
}

int Direction::getDY() const
{
	return dir > W || dir < E ? -1
		: dir > E && dir < W ? 1
		: 0;
}

Point Direction::getP() const
{
	return Point(getDX(), getDY());
}

Direction& Direction::operator=(Directions d)
{
	this->dir = d;
	return *this;
}

bool Direction::operator<(Directions d) const
{
	return this->dir < d;
}
