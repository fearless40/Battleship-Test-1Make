#ifndef FILEFUNCS_H
#define FILEFUNCS_H

#include "Helper.h"
#include "UserInput.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum LoadFileResult
{
    LFR_NoFileFound = 0,
    LFR_Success = 1,
    LFR_CORRUPT = -1,
    LFR_OUTOFMEM = -2
};

LoadFileResult load_file(const char *filename, bbboard *myboard);

#endif // !FILEFUNCS_H