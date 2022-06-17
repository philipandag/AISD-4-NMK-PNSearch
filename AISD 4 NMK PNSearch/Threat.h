#pragma once
#include "Field.h"

class Threat
{
public:
	Point pos;
	Field player;
	Threat* next;
	Threat* previous;

	Threat(Point p, Field pl):
	pos(p), player(pl), next(nullptr), previous(nullptr) 
	{}
	Threat(const Threat* other) {
		pos = other->pos;
		player = other->player;
		next = nullptr;
		previous = nullptr;
	}

};

class Threats {
public:
	Threat* rootP1;
	Threat* rootP2;
	int p1Length;
	int p2Length;


	Threats() {
		rootP1 = nullptr;
		rootP2 = nullptr;
		p1Length = 0;
		p2Length = 0;
	}
	~Threats()
	{
		deleteThreatList(&rootP1);
		deleteThreatList(&rootP2);
	}

	Threats(const Threats& other) {
		p1Length = other.p1Length;
		p2Length = other.p2Length;
		if (other.rootP1 == nullptr)
			rootP1 = nullptr;
		else
		{
			rootP1 = new Threat(other.rootP1);
			Threat* current = rootP1;
			Threat* currentOther = other.rootP1;
			if (currentOther != nullptr)
			{
				while (currentOther->next != nullptr) {
					current->next = new Threat(currentOther->next);
					current = current->next;
					currentOther = currentOther->next;
				}
				current->next = nullptr;
			}
		}
		if (other.rootP2 == nullptr)
			rootP2 = nullptr;
		else
		{
			rootP2 = new Threat(other.rootP2);
			Threat* current = rootP2;
			Threat* currentOther = other.rootP2;
			while (currentOther->next != nullptr) {
				current->next = new Threat(currentOther->next);
				current = current->next;
				currentOther = currentOther->next;
			}
			current->next = nullptr;
		}
	}

	void deleteThreatList(Threat** root) 
	{
		Threat* previous = *root;

		while (*root != nullptr)
		{
			*root = (*root)->next;
			delete previous;
			previous = *root;
		}
	}

	Threat* getRootP1() {
		return rootP1;
	}
	Threat* getRootP2() {
		return rootP2;
	}

	int getLengthP1() {
		return p1Length;
	}

	int getLengthP2() {
		return p2Length;
	}

	void add(Threat* threat) {
		if (threat->player == Field::P1) 
		{
			if(addTo(threat, &rootP1))
				p1Length++;
		}
		else if (threat->player == Field::P2)
		{
			if(addTo(threat, &rootP2))
				p2Length++;
		}
	}

	bool addTo(Threat* threat, Threat** selectedRoot)
	{
		if (*selectedRoot == nullptr)
		{
			*selectedRoot = threat;
			return true;
		}
		Threat* current = *selectedRoot;

		if (current->player == threat->player && current->pos == threat->pos)
		{
			delete threat;
			return false;
		}

		while (current->next != nullptr)
		{
			current = current->next;
			if (current->player == threat->player && current->pos == threat->pos)
			{
				delete threat;
				return false;
			}			
		}

		current->next = threat;
		threat->previous = current;
		return true;
	}

	bool removeFrom(Point at, Threat** selectedRoot)
	{
		if (*selectedRoot == nullptr)
			return false;
		Threat* current = *selectedRoot;

		while (current != nullptr) {
			if (current->pos == at)
			{
				removeThreat(&current);
				return true;
			}
			current = current->next;
		}
		return false;
	}

	void remove(Point at) 
	{
		if (removeFrom(at, &rootP1))
			p1Length--;

		if (removeFrom(at, &rootP2))
			p2Length--;
	}

	void removeThreat(Threat** t) {
		if ((*t)->previous == nullptr)
		{
			if ((*t)->next == nullptr)
			{
				if ((*t)->player == Field::P1)
					rootP1 = nullptr;
				else if ((*t)->player == Field::P2)
					rootP2 = nullptr;
			}
			else
			{
				if ((*t)->player == Field::P1)
				{
					rootP1 = (*t)->next;
					rootP1->previous = nullptr;
				}
				else if ((*t)->player == Field::P2)
				{
					rootP2 = (*t)->next;
					rootP2->previous = nullptr;
				}
			}
		}
		else
		{
			if ((*t)->next == nullptr)
			{
				(*t)->previous->next = nullptr;
			}
			else
			{
				(*t)->previous->next = (*t)->next;
				(*t)->next->previous = (*t)->previous;
			}
		}
		delete (*t);
	}
};


