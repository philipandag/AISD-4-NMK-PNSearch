#pragma once
#include <iostream>
#include "Direction.h"
#include "Point.h"

using namespace std;

extern const char* RESULTS[3];

enum Field { // Also used to determine winner, EMPTY = TIE
P2 = -1, 
EMPTY = 0,
P1 = 1,
};

istream& operator>>(istream& is, Field& f);
Field changePlayer();
Field findInArray(Field* array, int size, Field toFind);
Field getOtherPlayer(Field f);
Field getWinMove(Field player);
bool empty(Field f);
bool notChecked(Field player);
Field symbolToField(int n);
int fieldToSymbol(Field f);
Field** createFields(int n, int m);