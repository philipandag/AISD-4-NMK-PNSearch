#pragma once
#include "Field.h"

const int PN_MAX_VALUE = 100000;
const int PN_MIN_VALUE = 0;


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
};

enum NodeTypes {
	AND_NODE,
	OR_NODE
};

enum NodeValues {
	Win,
	Lose,
	Draw,
	Unknown
};

struct PNSearchNode {
	Board* board;
	int maxNodes;
	NodeTypes type;
	bool expanded;
	PNSearchNode* parent;
	PNSearchNode* sibling;
	NodeValues value;
	int proof;
	int disproof;
	int childrenAmount;
	PNSearchNode** children;
	int changeX;
	int changeY;
	Field player;

	PNSearchNode(Board* board, int x = -1, int y = -1);
	~PNSearchNode();
	void deleteSubtree();
	void set();
	void unset();
};

PNSearchNode* updateAncestors(PNSearchNode* node, PNSearchNode* root);
void expandNode(PNSearchNode* node);
PNSearchNode* selectMostProvingNode(PNSearchNode* node);
void setProofAndDisproofNumbers(PNSearchNode* node);
void generateAllChildren(PNSearchNode* node);
void PN(PNSearchNode* root);
void evaluate(PNSearchNode* node);
bool PNDraw(PNSearchNode* root);
