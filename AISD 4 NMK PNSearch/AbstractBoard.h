#pragma once
#include "Field.h"
#include "Threat.h"
class AbstractBoard {
public:
	virtual Field checkWin() = 0;
	virtual Field checkWinningSituations() = 0;
	virtual Field checkWinAt(Point pos) = 0;
	virtual Field checkWinAround(Point pos) = 0;
	virtual int getEmptyFields() = 0;
	virtual AbstractBoard& set(Point p, Field value) = 0;
	virtual void changePlayer() = 0;
	virtual int getN() = 0;
	virtual int getM() = 0;
	virtual Field& at(Point p) = 0;
	virtual Field getPlayer() = 0;
	virtual void updateWinningSituationsAt(Point pos, Threats& threats) = 0;
	virtual void updateAllWinningSituations(Threats& threats) = 0;
	virtual Field checkWinningSituationsSmart(Field currentPlayer, Threats& threats) = 0;
};
