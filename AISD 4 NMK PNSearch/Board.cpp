#include "Board.h"


Board::Board(int n, int m, int k, Field player):
	n(n), m(m), k(k), player(player), fields(createFields(n, m)), emptyFields(0)
{}

Board::Board():
	n(0), m(0), k(0), player(Field::EMPTY), fields(nullptr), emptyFields(0)
{}

Board::Board(Board& b):
	n(b.n), m(b.n), k(b.k), player(b.player), fields(createFields(n, m)), emptyFields(b.emptyFields)
{
	for (int y = 0; y < b.n; y++)
		for (int x = 0; x < b.m; x++)
			fields[y][x] = b.fields[y][x];
}

Field Board::checkWin()
{
	Field win = Field::EMPTY;

	for (int y = 0; y < n; y++)
		for (int x = 0; x < m; x++)
		{
			win = checkWinAt({ x, y });
			if (win == Field::P1 || win == Field::P2)
				return win;
		}
	return Field::EMPTY;
}

Field Board::checkWinAt(Point pos)
{
	if ((*this)[pos] == Field::EMPTY)
		return Field::EMPTY;

	for (Direction d = Direction::E; d < Direction::W; ++d) // >, _\, \/, /_
		if (countInDirection(pos, d.getP()) >= k)
			 return at(pos);

	return Field::EMPTY;
}

Field Board::checkWinAround(Point pos)
{
	if ((*this)[pos] == Field::EMPTY)
		return Field::EMPTY;
	Direction d = Direction::N;
	for (int i = 0; i < Direction::DIRECTIONS_AMOUNT; ++d, ++i)
		if (countInDirection(pos, d.getP()) >= k)
			return at(pos);
	return Field::EMPTY;
}

bool Board::onBoard(Point p) const
{
	return p.x >= 0 && p.x < m && p.y >= 0 && p.y < n;
}

int Board::countInDirection(Point p, Point delta)
{
	if (!onBoard(p) || empty(at(p)))
		return 0;

	int counter = 1;
	Field first = at(p);
	while (onBoard(p + delta * counter) && at(p + delta * counter) == first){
		counter++;
	}
	return counter;
}

Field* Board::operator[](size_t index) {
	return fields[index];
}

const Field* Board::operator[](size_t index) const {
	return fields[index];
}

ostream& operator<<(ostream& os, const Board& board)
{
	for (int y = 0; y < board.n; y++)
	{
		for (int x = 0; x < board.m; x++)
			printf_s("%d ", fieldToSymbol(board.fields[y][x]));
		printf_s("\n");
	}
	return os;
}

Board& Board::read()
{
	if (fields != nullptr)
		deleteFields();
	int playerSym;
	scanf_s("%d %d %d %d", &n, &m, &k, &playerSym);
	player = symbolToField(playerSym);

	fields = createFields(n, m);
	emptyFields = 0;

	int f;
	for (int y = 0; y < n; y++)
		for (int x = 0; x < m; x++)
		{
			scanf_s("%d", &f);
			fields[y][x] = symbolToField(f);
			if (empty(fields[y][x]))
				emptyFields++;
		}

	return *this;
}

void Board::deleteFields()
{
	for (int i = 0; i < n; i++)
		delete fields[i];
	delete fields;
}

void Board::generateMoves()
{
	if (checkWin() != Field::EMPTY)
	{
		printf_s("0\n");
		return;
	}

	printf_s("\n%d\n", emptyFields);

	for (int y = 0; y < n; y++)
		for (int x = 0; x < m; x++)
			if (empty(fields[y][x]))
			{
				fields[y][x] = player;
				print();
				fields[y][x] = Field::EMPTY;
			}
}

Board::~Board()
{
	deleteFields();
}

Field Board::operator[](Point p) const
{
	return fields[p.y][p.x];
}

Field& Board::operator[](Point p)
{
	return fields[p.y][p.x];
}

Field& Board::at(Point p) 
{
	return fields[p.y][p.x];
}

void Board::generateMovesCut()
{
	if (checkWin() != Field::EMPTY)
	{
		fprintf_s(stdout, "0\n");
		return;
	}

	for (int y = 0; y < n; y++)
		for (int x = 0; x < m; x++)
			if (empty(fields[y][x]))
			{
				fields[y][x] = player;
				if (checkWin())
				{
					printf_s("1\n");
					print();
					return;
				}
				fields[y][x] = Field::EMPTY;
			}

	generateMoves();
}

void Board::solveGame()
{
	Field result = solve();
	printf_s("%s\n", RESULTS[fieldToSymbol(result)]);
}

void Board::solveGamePNS()
{
	Field result = PNSearchSolve();
	printf_s("%s\n", RESULTS[fieldToSymbol(result)]);
}

Board& Board::set(Point p, Field value)
{
	if (at(p) != value)
	{
		if (value == Field::EMPTY)
			emptyFields++;
		else
			emptyFields--;
	}
	at(p) = value;

	return *this;
}


Field Board::solve()
{
	Field win = checkWin();
	if (win == Field::P1 || win == Field::P2)
		return win;
	win = checkWinningSituations();
	if (win == Field::P1 || win == Field::P2)
		return win;

	if (emptyFields == 0)
		return Field::EMPTY;

	Field otherPlayer = getOtherPlayer(player);
	Field best = otherPlayer; // other player wins - the worst scenario

	int resultNumber = 0;
	for (int y = 0; y < n; y++)
		for (int x = 0; x < m; x++)
			if (fields[y][x] == Field::EMPTY && resultNumber < emptyFields)
			{
				Point pos = { x, y };
				set(pos, player);
				player = getOtherPlayer(player);
				Field result = solve();
				set({ x, y }, Field::EMPTY);
				player = getOtherPlayer(player);

				if (result == player) // quick optimisation - if its the best option - it would be chosen no matter what the others would be, so don't bother checking them
					return player;

				if (result == Field::EMPTY && best == otherPlayer)
					best = Field::EMPTY;

				resultNumber++;
			}

	return best;
}

Field Board::checkWinningSituations()
{
	Point pos;
	Point delta;
	int P1km1Sequences = 0;
	int P2km1Sequences = 0;

	Field frontField = Field::EMPTY;
	int frontCounter = 0;
	Field backField = Field::EMPTY;
	int backCounter = 0;

	for (int y = 0; y < n; y++)
	{
		for (int x = 0; x < m; x++)
		{
			pos = { x,y };
			if (empty(at(pos)))
			{
				bool p1Incremented = false;
				bool p2Incremented = false;
				for (Direction d = Direction::E; d < Direction::W; ++d)
				{
					delta = d.getP();

					frontCounter = countInDirection(pos + delta, delta);
					backCounter = countInDirection(pos - delta, -delta);
					frontField = frontCounter > 0 ? at(pos + delta) : Field::EMPTY;
					backField = backCounter > 0 ? at(pos - delta) : Field::EMPTY;

					if (frontCounter == k)
						return frontField;
					if (backCounter == k)
						return backField;
					
					km1SequencesIncrementer(frontField, frontCounter, backField, backCounter, P1km1Sequences, P2km1Sequences, p1Incremented, p2Incremented);
				}
			}
		}
	}
	return km1SequencesGetWinner(P1km1Sequences, P2km1Sequences);
}

void Board::km1SequencesIncrementer(Field frontField, int frontCounter, Field backField, int backCounter, int& P1km1Sequences, int& P2km1Sequences, bool& p1Incremented, bool& p2Incremented) const
{
	if (frontField == backField && frontCounter + backCounter >= (k - 1)) {
		incrementerTryToIncrement(frontField, P1km1Sequences, P2km1Sequences, p1Incremented, p2Incremented);
	}
	else
	{
		if (frontCounter == (k - 1)) {
			incrementerTryToIncrement(frontField, P1km1Sequences, P2km1Sequences, p1Incremented, p2Incremented);
		}

		if (backCounter == (k - 1)) {
			incrementerTryToIncrement(backField, P1km1Sequences, P2km1Sequences, p1Incremented, p2Incremented);
		}
	}
}

void Board::incrementerTryToIncrement(Field f, int& P1km1Sequences, int& P2km1Sequences, bool& p1Incremented, bool& p2Incremented) const
{
	if (f == P1 && p1Incremented == false) {
		P1km1Sequences++;
		p1Incremented = true;
	}
	else if (f == P2 && p2Incremented == false) {
		P2km1Sequences++;
		p2Incremented = true;
	}
}

Field Board::km1SequencesGetWinner(int P1Sequences, int P2Sequences) const{
	if (player == P1 && P1Sequences >= 1)
		return P1;
	if (player == P2 && P2Sequences >= 1)
		return P2;
	if (P1Sequences >= 2)
		return P1;
	if (P2Sequences >= 2)
		return P2;

	return Field::EMPTY;
}

void Board::print() 
{
	for (int y = 0; y < n; y++)
	{
		for (int x = 0; x < m; x++) {
			printf_s("%d ", fieldToSymbol(fields[y][x]));
		}
		printf_s("\n");
	}
	printf_s("\n");
}


//#####################################################        PNS        #############################################################


Field Board::PNSearchSolve()
{
	Field playerCopy = player;
	PNSearchNode root(this);
	root.player = getOtherPlayer(player);
	PN(&root);
	if (root.proof == 0)
		return playerCopy;
	else
	{
		/*
		player = playerCopy; // after PN the player tends to change, so lets bring the initial one back
		if (PNDraw(&root))
			return Field::EMPTY;
		else
			return getOtherPlayer(player);
			*/
	}
	return Field::EMPTY;
}


PNSearchNode::PNSearchNode(Board* boardPointer, int x, int y)
{
	board = boardPointer;
	maxNodes = board->emptyFields;
	expanded = false;
	parent = nullptr;
	value = Unknown;
	proof = 1;
	disproof = 1;
	childrenAmount = 0;
	children = new PNSearchNode*[maxNodes];
	for (int i = 0; i < maxNodes; i++)
		children[i] = nullptr;
	changeX = x;
	changeY = y;
	type = OR_NODE;

}

void PNSearchNode::deleteSubtree() {
	for (int i = 0; i < childrenAmount; i++)
		delete children[i];
	delete children;
	childrenAmount = 0;
}

PNSearchNode::~PNSearchNode()
{
	deleteSubtree();
}

void PNSearchNode::set() 
{
	if (changeX != -1 && changeY != -1)
	{
		board->set({ changeX, changeY }, player);
		board->player = getOtherPlayer(player);
	}

}

void PNSearchNode::unset()
{
	if (changeX != -1 && changeY != -1)
	{
		board->set({ changeX, changeY }, Field::EMPTY);
		board->player = getOtherPlayer(player);
	}

}

void evaluate(PNSearchNode* node)
{
	Field result;
	if (node->parent == nullptr)
	{
		if (node->changeX != -1 && node->changeY != -1)
			result = node->board->checkWinAround({ node->changeX, node->changeY });
		else
			result = node->board->checkWin();

		if (result == Field::EMPTY)
			result = node->board->checkWinningSituations();
		if (result == node->board->player)
		{
				node->value = Win;

		}
		else if (result == getOtherPlayer(node->board->player))
		{

				node->value = Lose;

		}
	}
	else
	{
		result = node->board->checkWin();
		if (result == Field::EMPTY)
			result = node->board->checkWinningSituations();

		if (result == node->board->player)
		{

				node->value = Lose;

		}
		else if (result == getOtherPlayer(node->board->player))
		{

				node->value = Win;

		}
	}


	if (result == Field::EMPTY)
	{
		if (node->board->emptyFields <= 1)
			node->value = Draw;
		else
			node->value = Unknown;
	}
	
}

void PN(PNSearchNode* root) { // P1 wins
	evaluate(root);
	setProofAndDisproofNumbers(root);
	PNSearchNode* currentNode = root;
	PNSearchNode* mostProvingNode;
	while (root->proof != 0 && root->disproof != 0) {
		mostProvingNode = selectMostProvingNode(currentNode);
		expandNode(mostProvingNode);
		currentNode = updateAncestors(mostProvingNode, root);
	}
}

void generateAllChildren(PNSearchNode* node)
{
	int i = 0;
	PNSearchNode* previous = nullptr;
	for (int y = 0; y < node->board->n; y++)
		for (int x = 0; x < node->board->m; x++)
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

void setProofAndDisproofNumbers(PNSearchNode* node) {
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

PNSearchNode* selectMostProvingNode(PNSearchNode* node) {
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

void expandNode(PNSearchNode* node) {
	generateAllChildren(node);
	for (int i = 0; i < node->childrenAmount; i++) {
		PNSearchNode* n = node->children[i];
		evaluate(n);
		setProofAndDisproofNumbers(n);
		//Addition to original code
		if ((node->type == OR_NODE && n->proof == 0) ||
			(node->type == AND_NODE && n->disproof == 0))
			break;
	}
	node->expanded = true;
}

PNSearchNode* updateAncestors(PNSearchNode* node, PNSearchNode* root) {
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

bool PNDraw(PNSearchNode* root)
{


	/*
	PNSearchNode* current = root;
	while (true)
	{
		if (current->childrenAmount == 0)
			if (current->board->checkWin() == Field::EMPTY || current->board->checkWinningSituations() == Field::EMPTY)
				return true;
			else
				return false;
		PNSearchNode* bestChild = current->children[0];
		if (current->type == OR_NODE)
		{
			for (int i = 0; i < current->childrenAmount; i++)
			{
				if(current->children[0]->proof != 0)
					if (PNDraw(current->children[0]))
						return true;
			}
			return false;
		}
		else
		{
			for (int i = 0; i < current->childrenAmount; i++)
			{
				if (current->children[0]->proof != 0)
					if (!PNDraw(current->children[0]))
						return false;
			}
			return true;
		}

	}
	*/
	return false;
}