#include "Board.h"

Board::Board(int n, int m, int k, Field player):
	n(n), m(m), k(k), player(player), fields(createFields(n, m)), emptyFields(0)
{}

Board::Board():
	n(0), m(0), k(0), player(Field::EMPTY), fields(nullptr), emptyFields(0)
{}

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

Board& Board::set(Point p, Field value)
{
	if ((*this)[p] != value)
	{
		if (value == Field::EMPTY)
			emptyFields++;
		else
			emptyFields--;
	}
	(*this)[p] = value;

	return *this;
}

Field Board::PNSearchSolveGame()
{
	PNSearchTree pnst(this);

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