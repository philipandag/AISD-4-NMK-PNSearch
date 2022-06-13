#include "Field.h"

extern const char* RESULTS[3] = {
	"BOTH_PLAYERS_TIE",
	"FIRST_PLAYER_WINS",
	"SECOND_PLAYER_WINS",
};

Field** createFields(int n, int m)
{
	Field** board = new Field*[n];
	for (int i = 0; i < n; i++)
		board[i] = new Field[m];
	return board;
}

istream& operator>>(istream& is, Field& f)
{
	int a;
	if (is >> a)
		f = symbolToField(a);
	return is;
}

Field getOtherPlayer(Field f)
{
	return f == Field::P1 ? Field::P2 : Field::P1;
}

bool operator<(Field lesser, Field than)
{
	if (lesser == P1 && than == EMPTY)
		return false;
	if (lesser == EMPTY && than == P1)
		return true;
	return lesser < than;
}

bool operator>(Field greater, Field than)
{
	if (greater == P1 && than == EMPTY)
		return true;
	if (greater == EMPTY && than == P1)
		return false;
	return greater < than;
}

bool operator<=(Field lesser, Field than)
{
	return lesser == than || lesser < than;
}
bool operator>=(Field greater, Field than)
{
	return greater == than || greater > than;
}

bool empty(Field f)
{
	return f == EMPTY;
}

Field symbolToField(int n) 
{
	return n == 2 ? P2 : static_cast<Field>(n);
}

int fieldToSymbol(Field f)
{
	return f == P2 ? 2 : f;
}
