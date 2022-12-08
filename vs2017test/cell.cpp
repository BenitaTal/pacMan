#include "cell.h"

Cell::Cell(int r, int c)
{
	row = r;
	col = c;
	parent = nullptr; //no parent
}

Cell::Cell(int r, int c, Cell* p )
{
	row = r;
	col = c;
	parent = p; //no parent
}

