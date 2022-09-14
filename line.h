#ifndef LINE_H
#define LINE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct Line
{
    char *content;
    char *comment;
    int alignment;
} Line;

Line parseLine(char *line);
char *trimString(char *str);
bool isLabel(char *line);
char *indent(char *str);
void setAlignment(Line *line_p, int max_content_size);

void freeLines(Line *lines, int num_lines);

#endif