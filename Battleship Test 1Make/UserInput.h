#ifndef USERINPUT_H
#define USERINPUT_H

#include "Helper.h"

void evaluate_input(char *tmpbuf, bbboard *myboard);
void evaluate_cmd_line(int argc, char *argv[], bbboard *myboard);
void output_string(const char *mystring);
void process(char *tmpbuf, bbboard *myboard);

#endif // USERINPUT_H
