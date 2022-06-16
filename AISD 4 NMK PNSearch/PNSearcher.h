#pragma once
#include "AbstractBoard.h"

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
		PNSearchNode* sibling;
		NodeValues value;
		int proof;
		int disproof;
		int childrenAmount;
		PNSearchNode** children;
		int changeX;
		int changeY;
		Field player;

		PNSearchNode(AbstractBoard* board, int x = -1, int y = -1);
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

	Field solve();
private:
	void evaluate(PNSearchNode* node);
	void expandNode(PNSearchNode* node);
	void setProofAndDisproofNumbers(PNSearchNode* node);
	bool PNDraw(PNSearchNode* root);
	void generateAllChildren(PNSearchNode* node);
	void PN(PNSearchNode* root);
	PNSearchNode* selectMostProvingNode(PNSearchNode* node);
	PNSearchNode* updateAncestors(PNSearchNode* node, PNSearchNode* root);
};