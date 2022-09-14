#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>
#include "line.h"

void setFileNames(char **file_names, int argc, char *argv[]);
void formatFile(char *name);
int nextLineLength(FILE *read_p);
void writeFormattedFile(char *name, Line *lines, int num_lines);
void freeStrings(char **strings, int num_strings);

#endif