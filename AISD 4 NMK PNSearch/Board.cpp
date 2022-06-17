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
	delete[] fields;
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

//use after placing the move at pos
void Board::updateWinningSituationsAt(Point pos, Threats& threats)
{


	for (int i = 0; i < Direction::DIRECTIONS_AMOUNT/2; i++)
	{
		Direction d1 = static_cast<Direction::Directions>(i);
		Direction d2 = static_cast<Direction::Directions>(i + Direction::DIRECTIONS_AMOUNT/2);
		Point delta1 = { d1.getDX(), d1.getDY() };
		Point delta2 = { d2.getDX(), d2.getDY() };
		Point empty1 = { -1, -1 };
		Point empty2 = { -1, -1 };
		int length1 = 0;
		int length2 = 0;
		int lengthAfterEmpty1 = 0;
		int lengthAfterEmpty2 = 0;

		for (int i = 1; i < k; i++)
		{
			if (!onBoard(pos + delta1 * i))
				break;;
			if (at(pos + delta1 * i) == Field::EMPTY)
			{
				if (empty1.x == -1 && empty1.y == -1)
				{
					empty1 = pos + delta1 * i;
				}
				else
				{
					break;
				}
			}
			else if (at(pos + delta1 * i) == at(pos))
			{
				if (empty1.x != -1 && empty1.y != -1)
					lengthAfterEmpty1++;
				else
					length1++;
			}
			else
				break;
		}
		for (int i = 1; i < k; i++)
		{
			if (!onBoard(pos + delta2 * i))
				break;;
			if (at(pos + delta2 * i) == Field::EMPTY)
			{
				if (empty2.x == -1 && empty2.y == -1)
				{
					empty2 = pos + delta2 * i;
				}
				else
				{
					break;
				}
			}
			else if (at(pos + delta2 * i) == at(pos))
			{
				if (empty2.x != -1 && empty2.y != -1)
					lengthAfterEmpty2++;
				else
					length2++;
			}
			else
				break;
		}
		if (1 + length1 + length2 == (k - 1))
		{
			if (empty1.x != -1 && empty1.y != -1)
				threats.add(new Threat(empty1, at(pos)));
			if (empty2.x != -1 && empty2.y != -1)
				threats.add(new Threat(empty2, at(pos)));
		}
		if(1 + length1 + lengthAfterEmpty1 == (k-1))
			if (empty1.x != -1 && empty1.y != -1)
				threats.add(new Threat(empty1, at(pos)));
		if(1  + length2 + lengthAfterEmpty2 == (k-1))
			if (empty2.x != -1 && empty2.y != -1)
				threats.add(new Threat(empty2, at(pos)));
		
	}
}

void Board::updateAllWinningSituations(Threats& threats)
{
	for (int y = 0; y < n; y++)
		for (int x = 0; x < m; x++)
			if(!empty(at({x, y})))
				updateWinningSituationsAt({ x, y }, threats);
}
Field Board::checkWinningSituationsSmart(Field currentPlayer, Threats& threats)
{
	int p1 = threats.getLengthP1();
	int p2 = threats.getLengthP2();

	if (p1 > 0 && currentPlayer == Field::P1)
		return Field::P1;
	if (p2 > 0 && currentPlayer == Field::P2)
		return Field::P2;

	if (p1 > 1)
		return Field::P1;
	if (p2 > 1)
		return Field::P2;
	return Field::EMPTY;
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

void Board::incrementerTryToIncrement(Field f, int& P1km1Sequences, int& P2km1Sequences, bool& p1Incremented, bool& p2Incremented)
{
	if (f == P1 && p1Incremented == false) {
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
	PNSearcher searcher(this);
	return searcher.solve();
}

int Board::getEmptyFields() {
	return emptyFields;
}

void Board::changePlayer() {
	player = getOtherPlayer(player);
}

int Board::getN() 
{
	return n;
}

int Board::getM()
{
	return m;
}

Field Board::getPlayer()
{
	return player;
}
