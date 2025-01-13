// Battleship Test 1Make.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "Helper.h"
#include <iostream>

// TODO: Reference additional headers your program requires here.
void query_array(bbboard *myboard, char *opbuf, int array_choice = 1);

/**
 * @brief Query a single Excel style value or Col,row value and put the result into result
 * @param myboard (in) ptr to loaded data
 * @param query (in) ptr to string to parse must be a null terminated string or undefined behavior will occur (crash)
 * @param result (out) int value to get
 * @return bool indicating success or failure (oob will be false)
 */
bool query_arg(bbboard *myboard, char *query, int &result);
