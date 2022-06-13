#pragma once
#include "Board.h"
#include "Field.h"

const int PN_MAX_VALUE = 1000;
const int PN_MIN_VALUE = 0;

struct PNSearchNode {
	Board* node;
	PNSearchNode** children;
	int maxChildrenAmount;
	int childrenAmount;
	PNSearchNode* parent;
	Field playerBeingChecked;
	int proof;
	int disproof;
	bool isOrNode; // false if is an and Node
	bool isLeaf;

	PNSearchNode(Board* board, bool orNode = true) 
	{
		node = board;
		maxChildrenAmount = board->emptyFields;
		childrenAmount = 0;
		children = new PNSearchNode * [childrenAmount];
		playerBeingChecked = board->player;
		parent = nullptr;
		proof = 1;
		disproof = 1;
		bool isOrNode = orNode;
		bool isLeaf = true;
	}

	void evaluate()
	{
		if (isLeaf) {
			return;
		}
		Field result = node->checkWin();
		if (result == playerBeingChecked)
		{
			proof = PN_MAX_VALUE;
			disproof = PN_MIN_VALUE;
		}
		else if (result == getOtherPlayer(playerBeingChecked))
		{

		}
	}

	void setProofAndDisproofNumbers()
	{
		if (isOrNode)
		{
			disproof = 0;
			for (int i = 0; i < childrenAmount; i++)
			{
				if (children[i] != nullptr)
				{
					disproof += children[i]->disproof;
					if (children[i]->proof < proof)
						proof = children[i]->proof;
				}
			}
		}
		else
		{
			proof = 0;
			for (int i = 0; i < childrenAmount; i++)
			{
				if (children[i] != nullptr)
				{
					proof += children[i]->proof;
					if (children[i]->disproof < disproof)
						disproof = children[i]->disproof;
				}
			}
		}
	}
};

class PNSearchTree {
	PNSearchNode* root;
public:

	PNSearchTree(Board* board) 
	{
		PNSearchNode root(board);
		int i = 0;
		for (int y = 0; y < board->n; y++)
			for (int x = 0; x < board->m; x++)
				if (empty(*board[y][x]))
				{
					*board[y][x] = board->player;
					root.children[i] = new PNSearchNode(board);
					*board[y][x] = Field::EMPTY;
					i++;
				}
	}

	bool doesP1Win()
	{
		root->evaluate();
		setProofAndDisproofNumbers(root);

		while (root.proof != 0 && root.disproof != 0
			&& countNodes() <= maxnodes)
		{
			//Second Part of the algorithm
			mostProvingNode = selectMostProvingNode(currentNode);
			expandNode(mostProvingNode);
			currentNode = updateAncestors(mostProvingNode, root);
		}
	}

	bool doesP2Win()
	{

	}
};