#include "PNSearcher.h"

PNSearcher::PNSearcher(AbstractBoard* board) {
	this->board = board;
	root = new PNSearchNode(board);
	firstPlayer = board->getPlayer();
}

Field PNSearcher::solve()
{
	root->player = getOtherPlayer(firstPlayer);
	PN(root);
	if (root->proof == 0)
		return firstPlayer;
	else
	{
		//board->setPlayer(firstPlayer);
		if (PNDraw(root))
			return Field::EMPTY;
		else
			return getOtherPlayer(firstPlayer);
	}
}

void PNSearcher::PN(PNSearcher::PNSearchNode* root) { // P1 wins
	evaluate(root);
	setProofAndDisproofNumbers(root);
	PNSearchNode* currentNode = root;
	while (root->proof != 0 && root->disproof != 0) {
		PNSearchNode* mostProvingNode = selectMostProvingNode(currentNode);
		expandNode(mostProvingNode);
		currentNode = updateAncestors(mostProvingNode, root);
	}
}

void PNSearcher::evaluate(PNSearcher::PNSearchNode* node)
{
	Field result;

	if (node->changeX != -1 && node->changeY != -1)
		result = node->board->checkWinAround({ node->changeX, node->changeY });
	else
		result = node->board->checkWin();

	if (result == Field::EMPTY)
		result = node->board->checkWinningSituations();
	if (result == firstPlayer)
	{
		node->value = Win;
	}
	else if (result == getOtherPlayer(firstPlayer))
	{
		node->value = Lose;
	}

	if (result == Field::EMPTY)
	{
		if (node->board->getEmptyFields() == 0)
			node->value = Draw;
		else
			node->value = Unknown;
	}

}

void PNSearcher::expandNode(PNSearcher::PNSearchNode* node) {
	generateAllChildren(node);
	for (int i = 0; i < node->childrenAmount; i++) {
		PNSearchNode* n = node->children[i];
		n->set();
		evaluate(n);
		setProofAndDisproofNumbers(n);
		n->unset();
		//Addition to original code
		if ((node->type == OR_NODE && n->proof == 0) ||
			(node->type == AND_NODE && n->disproof == 0))
			break;
	}
	node->expanded = true;
}

void PNSearcher::setProofAndDisproofNumbers(PNSearcher::PNSearchNode* node) {
	if (node->expanded) //Internal node;
		if (node->type == AND_NODE) {
			node->proof = 0;
			node->disproof = PN_MAX_VALUE;
			for (int i = 0; i < node->childrenAmount; i++) {
				PNSearchNode* n = node->children[i];
				node->proof = node->proof + n->proof <= PN_MAX_VALUE ? node->proof + n->proof : PN_MAX_VALUE;
				if (n->disproof < node->disproof)
					node->disproof = n->disproof;
			}
		}
		else { //OR node
			node->proof = PN_MAX_VALUE;
			node->disproof = 0;
			for (int i = 0; i < node->childrenAmount; i++) {
				PNSearchNode* n = node->children[i];
				node->disproof = node->disproof + n->disproof <= PN_MAX_VALUE ? node->disproof + n->disproof : PN_MAX_VALUE;
				if (n->proof < node->proof)
					node->proof = n->proof;
			}
		}
	else //Leaf
		switch (node->value) {
		case Lose:
		case Draw:
			node->proof = PN_MAX_VALUE;
			node->disproof = PN_MIN_VALUE;
			break;
		case Win:
			node->proof = PN_MIN_VALUE;
			node->disproof = PN_MAX_VALUE;
			break;
		case Unknown:
			node->proof = 1;
			node->disproof = 1;
		}
}

bool PNSearcher::PNDraw(PNSearcher::PNSearchNode* root)
{
	PNSearchNode* current = root;
	while (true)
	{
		if (current->childrenAmount == 0)
		{
			if (current->value == Unknown)
			{
				evaluate(current);
			}
			if (current->value == Unknown)
			{
				generateAllChildren(current);
				return PNDraw(current);
			}
			return current->value == Draw || current->value == Win;
		}

		if (current->type == OR_NODE)
		{
			for (int i = 0; i < current->childrenAmount; i++)
			{
				current->children[i]->set();
				if (PNDraw(current->children[i]))
				{
					current->children[i]->unset();
					return true;
				}
				current->children[i]->unset();
			}
			return false;

		}
		else
		{
			for (int i = 0; i < current->childrenAmount; i++)
			{
				current->children[i]->set();
				if (current->children[i]->proof != 0)
					if (!PNDraw(current->children[i]))
					{
						current->children[i]->unset();
						return false;
					}
				current->children[i]->unset();
			}
			return true;
		}

	}

	return false;
}

void PNSearcher::generateAllChildren(PNSearcher::PNSearchNode* node)
{
	int i = 0;
	PNSearchNode* previous = nullptr;
	for (int y = 0; y < node->board->getN(); y++)
		for (int x = 0; x < node->board->getM(); x++)
		{
			if (empty(node->board->at({ x, y })))
			{
				node->children[i] = new PNSearchNode(node->board, x, y);
				node->children[i]->maxNodes -= 1;
				node->children[i]->parent = node;
				node->children[i]->player = getOtherPlayer(node->player);
				node->children[i]->type = node->type == OR_NODE ? AND_NODE : OR_NODE;
				i++;
			}
		}
	node->childrenAmount = i;
}

PNSearcher::PNSearchNode* PNSearcher::selectMostProvingNode(PNSearcher::PNSearchNode* node) {
	node->set();
	while (node->expanded) {
		int i = 0;
		PNSearchNode* n = node->children[i++];
		if (node->type == OR_NODE) //OR Node
		{
			while (n->proof != node->proof)
				n = node->children[i++];
		}
		else //AND Node
		{
			while (n->disproof != node->disproof)
				n = node->children[i++];
		}
		node = n;
		node->set();
	}

	return node;
}

PNSearcher::PNSearchNode* PNSearcher::updateAncestors(PNSearcher::PNSearchNode* node, PNSearcher::PNSearchNode* root) {
	do {
		int oldProof = node->proof;
		int oldDisProof = node->disproof;

		setProofAndDisproofNumbers(node);

		//No change on the path
		if (node->proof == oldProof && node->disproof == oldDisProof)
			return node;

		//Delete (dis)proved trees
		//if (node->proof == 0 || node->disproof == 0)
			//node->deleteSubtree();

		if (node == root)
			return node;

		node->unset();
		node = node->parent;
	} while (true);
}

PNSearcher::PNSearchNode::PNSearchNode(AbstractBoard* boardPointer, int x, int y)
{
	board = boardPointer;
	maxNodes = board->getEmptyFields();
	expanded = false;
	parent = nullptr;
	value = Unknown;
	proof = 1;
	disproof = 1;
	childrenAmount = 0;
	children = new PNSearchNode * [maxNodes];
	for (int i = 0; i < maxNodes; i++)
		children[i] = nullptr;
	changeX = x;
	changeY = y;
	type = OR_NODE;
}

void PNSearcher::PNSearchNode::deleteSubtree() {
	for (int i = 0; i < childrenAmount; i++)
		delete children[i];
	delete children;
	childrenAmount = 0;
}

PNSearcher::PNSearchNode::~PNSearchNode()
{
	deleteSubtree();
}

void PNSearcher::PNSearchNode::set()
{
	if (changeX != -1 && changeY != -1)
	{
		board->set({ changeX, changeY }, player);
		board->changePlayer();
	}
}

void PNSearcher::PNSearchNode::unset()
{
	if (changeX != -1 && changeY != -1)
	{
		board->set({ changeX, changeY }, Field::EMPTY);
		board->changePlayer();
	}
}