#pragma once
#include "AbstractBoard.h"
#include "PNSearcher.h"


class Board : AbstractBoard
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
	Board(Board& b);
	Field* operator[](size_t index);
	const Field* operator[](size_t index) const;
	void print();
	Field checkWin();
	Field checkWinningSituations();
	Field checkWinAt(Point pos);
	Field checkWinAround(Point pos);
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
	Field PNSearchSolve();
	void solveGamePNS();
	bool doesP1Win();
	bool doesP2Win();
	int getEmptyFields() override;
	void changePlayer() override;
	int getN() override;
	int getM() override;
	Field getPlayer() override;
	void setPlayer(Field player) override;
};
