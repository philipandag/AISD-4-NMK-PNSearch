#include "Board.h"
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

const int INPUT_BUFFER_SIZE = 100;

int main()
{
	char* command = new char[INPUT_BUFFER_SIZE];
	while (cin >> command)
	{
		if (strcmp(command, "GEN_ALL_POS_MOV") == 0)
		{
			Board board;
			board.read();
			board.generateMoves();
		} 
		else if (strcmp(command, "GEN_ALL_POS_MOV_CUT_IF_GAME_OVER") == 0)
		{
			Board board;
			board.read();
			board.generateMovesCut();
		}
		else if (strcmp(command, "SOLVE_GAME_STATE") == 0)
		{
			Board board;
			board.read();
			board.solveGamePNS();

		}
		else if (strcmp(command, "PNS") == 0)
		{
			Board board;
			board.read();
			board.solveGamePNS();
		}

	}
	delete[] command;
}