#pragma once
#include "AbstractBoard.h"
#include "Threat.h"

const int PN_MAX_VALUE = 100000;
const int PN_MIN_VALUE = 0;

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

class PNSearcher {
	struct PNSearchNode {
		AbstractBoard* board;
		int maxNodes;
		NodeTypes type;
		bool expanded;
		PNSearchNode* parent;
		NodeValues value;
		int proof;
		int disproof;
		int childrenAmount;
		PNSearchNode** children;
		int changeX;
		int changeY;
		Field player;
		Threats* threats;

		PNSearchNode(AbstractBoard* boardPointer, int x = -1, int y = -1);
		~PNSearchNode();
		void deleteSubtree();
		void set();
		void unset();
	};

	AbstractBoard* board;
	PNSearchNode* root;
	Field firstPlayer;
public:
	PNSearcher(AbstractBoard* board);
	~PNSearcher();

	Field solve();
private:
	void evaluate(PNSearchNode* node);
	void expandNode(PNSearchNode* node);
	static void setProofAndDisproofNumbers(PNSearchNode* node);
	bool PNDraw(PNSearchNode* node);
	static void generateAllChildren(PNSearchNode* node);
	void PN();
	PNSearchNode* selectMostProvingNode(PNSearchNode* node);
	PNSearchNode* updateAncestors(PNSearchNode* node);
};