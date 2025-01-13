// Battleship Test 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "FileFuncs.h"
#include "Helper.h"
#include "UserInput.h"
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print(char *begin, char *end)
{
    char *i = begin;
    while (i != end)
    {
        fputc(*i, stdout);
        ++i;
    }
}

char *match_white_space(char *next)
{
    // puts("whitespace");
    char *ret = next;
    while (*ret == '\0' || *ret == ' ' || *ret == '\t')
    {
        // fputc(*ret, stdout);
        ++ret;
    }
    return ret;
}

char *match_numbers(char *next)
{
    // puts("numbers");
    char *ret = next;
    while (*ret >= '0' && *ret <= '9')
    {
        // fputc(*ret, stdout);
        ++ret;
    }
    return ret;
}

char *match_lcase_letters(char *next)
{
    // puts("lcase a-z");
    char *ret = next;
    char lower = tolower((unsigned int)*ret);

    while (lower >= 'a' && lower <= 'z')
    {
        // fputc(lower, stdout);
        ++ret;
        lower = tolower((unsigned int)*ret);
    }
    return ret;
}

int fromBase26(char *begin, char *end)
{
    int acc = 0;
    for (char *i = begin; i != end; ++i)
    {
        size_t index = end - i;
        acc += pow(26, index - 1) * (tolower(*i) - 'a');
    }
    return acc;
}

int fromBase10(char *begin, char *end)
{
    int acc = 0;
    for (char *i = begin; i != end; ++i)
    {
        size_t index = end - i;
        acc += pow(10, index - 1) * (*i - '0');
    }
    return acc;
}

bool query_arg_comma(bbboard *myboard, char *query_arg, int &result)
{
    // White space already matched
    // skip ws processing
    char *begin = query_arg;
    // Match numbers until the comma
    int col = 0;
    int row = 0;

    {
        char *end = match_numbers(begin);
        if (end == begin)
        {
            return false;
        }
        col = fromBase10(begin, end);
        begin = end;
    }
    // Check for white space before the comma
    begin = match_white_space(begin);
    if (*begin != ',')
        return false;
    ++begin;
    begin = match_white_space(begin);
    {
        char *end = match_numbers(begin);
        if (end == begin)
        {
            return false;
        }
        row = fromBase10(begin, end);
    }

    return board_get_value(myboard, row, col, result);
}

bool query_arg(bbboard *myboard, char *query, int &result)
{
    int col = 0;
    int row = 0;
    char *next = match_white_space(query);
    {
        char *end = match_lcase_letters(next);
        if (end == next)
        {
            // attempt to parse comma format
            return query_arg_comma(myboard, next, result);
        }
        col = fromBase26(next, end);
        next = end;
    }
    // Scan for row now
    {
        char *end = match_numbers(next);
        if (end == next)
        {
            // Error
        }
        row = fromBase10(next, end);
        next = end;
    }

    return board_get_value(myboard, row, col, result);
}

/* query_array, will query and create a string with the output of each input*/
/* input types can be A1,A2,A3,B4  or A1*/
void query_array(bbboard *myboard, char *opbuf, int array_choice = 1)
{
    char *test_char = opbuf;
    char buf[LARGE_BUF_SIZE] = {0}; // Large buffer for output
    char tmpbuf[100];

    while (test_char != NULL && *test_char != '\0')
    {
        // Skip leading spaces
        while (*test_char == ' ' || *test_char == '\t')
            ++test_char;

        int row_index = 0;
        int col_index = 0;
        char row_buf[50] = {0};
        // char col_buf[1270] = {0};
        int valid_count = 0, col_valid_count = 0;
        // Keep track of the total size of the string in buf
        size_t buf_len = 0;

        // Parse col (letters)
        while (*test_char >= 'a' && *test_char <= 'z')
        {
            // col_buf[col_valid_count++] = *test_char;
            ++col_valid_count;
            col_index = (int)pow(26, col_index - 1) + ((*test_char) - 'a'); // Col A is 0 in 0-based logic
            ++test_char;
        }

        // Ensure at least one letter for col
        if (col_valid_count == 0)
        {
            snprintf(tmpbuf, sizeof(tmpbuf), "OOB,");
            if (buf_len + strlen(tmpbuf) < LARGE_BUF_SIZE)
            {
                strcat_s(buf, LARGE_BUF_SIZE - buf_len, tmpbuf);
                buf_len += strlen(tmpbuf);
            }
            while (*test_char != '\0' && *test_char != ' ' && *test_char != ',')
                ++test_char;
            continue;
        }

        // Parse row (digits)
        while (*test_char >= '0' && *test_char <= '9')
        {
            // row_buf[valid_count++] = *test_char;
            ++valid_count;
            row_index = (int)pow(10, row_index - 1) + (*test_char - '0'); // Accumulate column value
            ++test_char;
        }

        // Apply zero-based indexing after parsing the full number
        /*if (valid_count > 0)
        {
            row_index -= 1; // Convert to zero-based index
        }*/

        // Ensure at least one digit for row
        if (valid_count == 0)
        {
            snprintf(tmpbuf, sizeof(tmpbuf), "OOB,");
            if (buf_len + strlen(tmpbuf) < LARGE_BUF_SIZE)
            {
                strcat_s(buf, LARGE_BUF_SIZE - buf_len, tmpbuf);
                buf_len += strlen(tmpbuf);
            }
            while (*test_char != '\0' && *test_char != ' ' && *test_char != ',')
                ++test_char;
            continue;
        }

        // Bounds checking
        if (row_index >= myboard->rows || col_index < 0 || col_index >= myboard->columns || row_index < 0)
        {
            snprintf(tmpbuf, sizeof(tmpbuf), "OOB,");
            if (buf_len + strlen(tmpbuf) < LARGE_BUF_SIZE)
            {
                strcat_s(buf, LARGE_BUF_SIZE - buf_len, tmpbuf);
                buf_len += strlen(tmpbuf);
            }

            while (*test_char != '\0' && *test_char != ' ' && *test_char == ',')
                ++test_char;
            continue;
        }

        // Array arithmetic and output
        int location = (row_index * myboard->columns) + col_index;
        snprintf(tmpbuf, sizeof(tmpbuf), "%i,", myboard->mine[location]);
        if (buf_len + strlen(tmpbuf) < LARGE_BUF_SIZE)
        {
            strcat_s(buf, LARGE_BUF_SIZE, tmpbuf);
            buf_len += strlen(tmpbuf);
        }

        // Skip trailing spaces and commas
        while (*test_char == ' ' || *test_char == ',')
            ++test_char;
    }

    // Remove trailing comma
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == ',')
    {
        buf[len - 1] = '\0';
    }

    output_string(buf);
}

int main(int argc, char *argv[])
{
    char mybuf[MAX_INPUT];
    int args_in = 0;
    char *current = mybuf;        // Pointer for snprintf
    size_t remaining = MAX_INPUT; // size_t defining the remaining room in the string
    if (argc < 2)
    {
        output_string("Loaded with too few or no arguments.  Proceeding.");
    }

    bbboard myboard;                      // Declare the new board which will hold the arrays and types
    memset(&myboard, 0, sizeof(myboard)); // Clear out the myboard and fill it with only 0's.
    myboard.interaactive_go = false;

    evaluate_cmd_line(argc, argv, &myboard);

    while (myboard.interaactive_go)
    {
        fgets(mybuf, MAX_INPUT, stdin);
        evaluate_input(mybuf, &myboard);
    }
}
