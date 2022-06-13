#pragma once
#include "Field.h"
#include "PNSearchTree.h"
class Board
{
public:
	int n, m, k;
	Field player;
	Field** fields;
	int emptyFields;

	void deleteFields();
public:
	Board(int n, int m, int k, Field player);
	Board();
	Field* operator[](size_t index);
	const Field* operator[](size_t index) const;
	void print();
	Field checkWin();
	Field checkWinningSituations();
	Field checkWinAt(Point pos);
	bool onBoard(Point p) const;
	Field operator[](Point p) const;
	Field& operator[](Point p);
	Field& at(Point p);
	int countInDirection(Point p, Point delta);
	Board& read();
	void generateMoves();
	void generateMovesCut();
	Field solve();
	void solveGame();
	Board& set(Point p, Field value);
	void km1SequencesIncrementer(Field frontField, int frontCounter, Field backField, int backCounter, int& P1km1Sequences, int& P2km1Sequences, bool& p1Incremented, bool& p2Incremented) const;
	void incrementerTryToIncrement(Field f, int& P1km1Sequences, int& P2km1Sequences, bool& p1Incremented, bool& p2Incremented) const;
	Field km1SequencesGetWinner(int P1Sequences, int P2Sequences) const;
	~Board();
	Field PNSearchSolveGame();
	bool doesP1Win();
	bool doesP2Win();
};