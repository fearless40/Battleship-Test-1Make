#include "Helper.h"

bool board_get_value(bbboard *myboard, int row, int col, int &result)
{
    if (!myboard && myboard->mine)
        return false;

    if (row < 0 || row > myboard->rows || col < 0 || col > myboard->columns)
    {
        return false;
    }

    result = myboard->mine[(row * myboard->columns) + col];
    return true;
}