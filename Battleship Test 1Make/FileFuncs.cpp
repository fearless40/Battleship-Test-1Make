#include "FileFuncs.h"
#include "Helper.h"

/* Load_File will open a file, check it and load it into the bbboard struct
    It will return the following:
    0 = Failed to open the file
    1 = Success
    -1 = File is corrupt
    -2 = Memory is allocated badly
*/
LoadFileResult load_file(const char *filename, bbboard *myboard)
{

#define ERR(code)                                                                                                      \
    err_code = code;                                                                                                   \
    goto cleanup;

    FILE *myfile = NULL;
    char *posbuf = NULL;
    char *filebuf = NULL;
    LoadFileResult err_code = LFR_CORRUPT;

    size_t total_cells = 0;
    int max_row_size = 0;
    char *buf = NULL;
    int row_count = 0;

    if (fopen_s(&myboard->savefile, filename, "r") != 0)
    {
        myboard->savefile = NULL;
        ERR(LFR_NoFileFound)
    }

    if (!(fscanf_s(myboard->savefile, "%i %i", &myboard->rows, &myboard->columns) == 2))
    {
        puts("Unable to read row / col.");
        ERR(LFR_CORRUPT) // Failed to read the correct two lines (rows and columns) file is corrupt
    }

    if (myboard->rows <= 0 || myboard->columns <= 0 || myboard->rows > UINT16_MAX || myboard->columns > UINT16_MAX)
    {
        puts("Row and col are too big or small.");
        ERR(LFR_CORRUPT) // Rows/columns are too small or too large for unsigned int 16 bit
    }

    if (myboard->rows * myboard->columns > INT_MAX)
    {
        puts("Row*col too large.");
        ERR(LFR_CORRUPT) // The rows and columns are too big
    }

    // Dynamically allocate the memory for the array using pointer arithmetic
    total_cells = (size_t)myboard->rows * (size_t)myboard->columns;
    myboard->mine = (int *)malloc(sizeof(int) * total_cells);

    if (myboard->mine == NULL)
    {
        puts("Out of memory.");
        ERR(LFR_OUTOFMEM) // Memory allocation failed
    }

    // Create a buffer of size 6 (unsigned int max size including -) + comma * columns + 2 for \0 and \n
    max_row_size = 7 * myboard->columns + 2;

    // Create a temporary buffer that will contain the row of data and ,'s
    buf = (char *)malloc(max_row_size);
    if (buf == NULL)
    { // Memory allocation failed
        puts("Out of memory.");
        ERR(LFR_OUTOFMEM);
    }

    row_count = 0;
    while (row_count < myboard->rows)
    {
        if (fgets(buf, max_row_size, myboard->savefile) == NULL)
        { // It failed to read
            puts("Unable to read a line of input in.");
            ERR(LFR_CORRUPT)
        }

        // Remove newline character at the end of the buffer, if present
        size_t len = strlen(buf);
        if (len == 0) // Unexpected blank line, file is corrupt
        {
            puts("Blank line found in file.");
            ERR(LFR_CORRUPT)
        }
        if (len > 0 && buf[len - 1] == '\n' || buf[len - 1] == '\r')
        {
            buf[len - 1] = '\0'; // Replace newline with null terminator
        }
        if (strlen(buf) == 0 || buf[0] == '\0')
        { // Sometimes an empty buffer is sent, continue if that occurs, this happens sometimes in tokenization
            continue;
        }

        char *context = NULL;                       // Context for strtok_s
        char *token = strtok_s(buf, ",", &context); // Tokenize using strtok_s
        char *errptr;
        if (!token)
        { // File is corrupt
            puts("Invalid token found.");
            ERR(LFR_CORRUPT)
        }
        int col_count = 0;
        while (token != NULL)
        {
            if (col_count >= myboard->columns)
            { // Too many tokens data is corrupt
                puts("Number of columns in file more than specified.");
                ERR(LFR_CORRUPT)
            }
            myboard->mine[(row_count * myboard->columns) + col_count] = strtol(token, &errptr, 10);
            if (errptr == token || *errptr != '\0')
            { // The string contains no digits or non-digit characters
                puts("Invalid format of data value.");
                ERR(LFR_CORRUPT);
            }
            token = strtok_s(NULL, ",", &context); // Get next token
            ++col_count;
        }
        if (col_count != myboard->columns)
        { // Corrupt file, not enough tokens or too many tokens
            puts("Number of col is not equal to what is expected");
            ERR(LFR_CORRUPT)
        }
        row_count++;
    }
    free(buf);                 // Free malloc memory.
    fclose(myboard->savefile); // Shouldn't need the file anymore for this application.  Later may want to keep it.
    myboard->loaded = 1;
    myboard->interaactive_go = true;
    return LFR_Success;

cleanup:
    free(buf);
    free(myboard->mine);
    if (myboard->savefile)
        fclose(myboard->savefile);
    myboard->interaactive_go = false;
    return err_code;
}

#undef ERR
