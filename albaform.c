/* albaform.c

A formatter for albaCore assembly files.

Author: Evan Day
Date: September 8th, 2022
Estimate: 5 hours
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

typedef struct Line
{
    char *content;
    char *comment;
    int alignment;
} Line;

void setFileNames(char **file_names, int argc, char *argv[]);

void formatFile(char *name);

int nextLineLength(FILE *read_p);
Line parseLine(char *line);
char *trimString(char *str);
bool isLabel(char *line);
char *indent(char *str);
void setAlignment(Line *line_p, int max_content_size);

void writeFormattedFile(char *name, Line *lines, int num_lines);

void freeStrings(char **strings, int num_strings);
void freeLines(Line *lines, int num_lines);

int main(int argc, char *argv[])
{
    int num_files = argc < 2 ? 1 : argc - 1;

    char **file_names = malloc(num_files * sizeof(char *));

    setFileNames(file_names, argc, argv);

    for (int i = 0; i < num_files; ++i)
        formatFile(file_names[i]);

    freeStrings(file_names, num_files);

    return 0;
}

void setFileNames(char **file_names, int argc, char *argv[])
{
    // set file names
    if (argc <= 1) // no files passed. ask user for file name
    {
        // prompt user
        printf("Enter file to format: ");

        // scan user response
        const int filename_buffer_size = 64;
        *file_names = malloc(filename_buffer_size * sizeof(char));
        fgets(*file_names, filename_buffer_size, stdin);

        // remove newline
        char *p = *file_names;
        while (*p != '\n')
            p++;
        *p = 0;
    }
    else // open each file from argv
    {
        for (int i = 0; i < argc - 1; ++i)
        {
            file_names[i] = malloc(strlen(argv[i + 1]) * sizeof(char));
            strncpy(file_names[i], argv[i + 1], strlen(argv[i + 1]));
        }
    }
}

void freeStrings(char **strings, int num_strings)
{
    for (int i = 0; i < num_strings; ++i)
        if (strings[i] != NULL)
            free(strings[i]);

    free(strings);
}

void freeLines(Line *lines, int num_lines)
{
    for (int i = 0; i < num_lines; ++i)
    {
        if (lines[i].comment != NULL)
            free(lines[i].comment);

        if (lines[i].content != NULL)
            free(lines[i].content);
    }

    free(lines);
}

// trimString() expects a pointer to a buffer that was dynamically allocated
char *trimString(char *str)
{
    char *start = str;
    char *end = str;

    while (isspace(*start))
        start++;

    // check if string is all whitespace
    if (*start == 0)
    {
        free(str);
        return NULL;
    }

    // move end to end of string...
    while (*(end + 1))
        end++;

    // ...then move it back to the first non-whitespace character
    while (isspace(*end))
        end--;

    // make new string buffer
    int new_len;
    new_len = ((end - start) / sizeof(char)) + 1;

    char *new_str = malloc((new_len + 1) * sizeof(char)); // include space for null terminator

    // write new string
    strncpy(new_str, start, new_len);
    new_str[new_len] = 0;

    // free old string
    free(str);

    // point old string pointer to new string
    return new_str;
}

void formatFile(char *name)
{
    // open file
    FILE *read_p = fopen(name, "r");
    if (read_p == NULL)
    {
        printf("Unable to open file: %s\n", name);
        return;
    }

    // count lines in file
    int num_lines = 0;
    while (1)
    {
        char c = fgetc(read_p);

        if (c == '\n')
            num_lines++;

        if (c == EOF)
        {
            num_lines++;
            break;
        }
    }

    // return file pointer to start
    fseek(read_p, 0, SEEK_SET);

    // create lines array
    Line *lines = malloc(num_lines * sizeof(Line));

    // read file into lines array
    for (int i = 0; i < num_lines; ++i)
    {
        // scan line from file
        int temp_buffer_size = nextLineLength(read_p) + 1; // + 1 for null character
        char *temp_line_buffer = malloc(temp_buffer_size * sizeof(char));
        fgets(temp_line_buffer, temp_buffer_size, read_p);
        lines[i] = parseLine(temp_line_buffer);
    }

    // file is no longer needed
    fclose(read_p);

    bool underLabel = false;
    int max_content_size = 0;
    for (int i = 0; i < num_lines; ++i)
    {
        if (lines[i].content == NULL)
            continue;

        // update label indent
        if (isLabel(lines[i].content))
            underLabel = true;
        else if (underLabel)
            lines[i].content = indent(lines[i].content);

        // track max content length
        int temp_size = strlen(lines[i].content);
        if (temp_size > max_content_size)
            max_content_size = temp_size;
    }

    // align comment
    for (int i = 0; i < num_lines; ++i)
        setAlignment(&lines[i], max_content_size);

    writeFormattedFile(name, lines, num_lines);

    freeLines(lines, num_lines);
}

int nextLineLength(FILE *fp)
{
    // count characters in the line
    char c;
    int count = 0;
    while (1)
    {
        c = fgetc(fp);
        count++;
        if (c == EOF || c == '\n')
            break;
    }

    // return file pointer to start of line
    fseek(fp, -count, SEEK_CUR);

    return count;
}

bool isLabel(char *line)
{
    // do not indent .text, .data, etc.
    if (line[0] == '.')
        return true;

    for (char *p = line; *p; p++)
        if (*p == ':')
            return true;

    return false;
}

// indent() expects a pointer to a buffer that was dynamically allocated
char *indent(char *str)
{
    // alloc new string with increase size
    const int INDENT_SIZE = 4;
    char *new_str = malloc((strlen(str) + INDENT_SIZE + 1) * sizeof(char));

    // add the indent
    char *w = new_str;
    for (int i = 0; i < INDENT_SIZE; ++i)
    {
        *w = ' ';
        w++;
    }

    // copy the rest of the original string
    char *r = str;
    while (*r)
    {
        *w = *r;
        w++;
        r++;
    }
    *w = 0;

    // free old string
    free(str);

    // return new string
    return new_str;
}

void writeFormattedFile(char *name, Line *lines, int num_lines)
{
    FILE *fp = fopen(name, "w");

    for (int i = 0; i < num_lines; ++i)
    {
        if (lines[i].content != NULL)
            fprintf(fp, "%s", lines[i].content);

        for (int j = 0; j < lines[i].alignment; ++j)
            fprintf(fp, " ");

        if (lines[i].comment == NULL)
            fprintf(fp, "\n");
        else
            fprintf(fp, "//%s\n", lines[i].comment);
    }

    fclose(fp);
}

Line parseLine(char *line)
{
    bool hasComment = strstr(line, "//");

    Line parsed;

    if (!hasComment)
    {
        parsed.content = trimString(line);
        parsed.alignment = 0;
        parsed.comment = NULL;
        return parsed;
    }

    parsed.content = malloc((strlen(line) + 1) * sizeof(char));
    parsed.comment = malloc((strlen(line) + 1) * sizeof(char));

    char *r = line;
    char *w = parsed.content;

    // copy content
    while (*r)
    {
        if (*r == '/' && *(r + 1) == '/')
            break;

        *w = *r;
        w++;
        r++;
    }
    *w = 0;

    // copy comment
    r += 2; // move past comment indicator
    w = parsed.comment;
    while (*r)
    {
        *w = *r;
        w++;
        r++;
    }
    *w = 0;

    // remove newline from comment
    r = parsed.comment;
    while (*r)
        r++;
    r--;
    *r = 0;

    parsed.content = trimString(parsed.content);
    parsed.comment = realloc(parsed.comment, (strlen(parsed.comment) + 1) * sizeof(char));

    // add space to start if there isn't one
    if (!isspace(*parsed.comment))
    {
        parsed.comment = realloc(parsed.comment, (strlen(parsed.comment) + 2) * sizeof(char));

        // remove newline
        r = parsed.comment;
        while (*r)
            r++;

        w = r + 1;

        while (r >= parsed.comment)
        {
            *w = *r;
            r--;
            w--;
        }

        *w = ' ';
    }

    free(line);

    return parsed;
}

void setAlignment(Line *line_p, int max_content_size)
{
    if (line_p->comment == NULL)
    {
        line_p->alignment = 0;
        return;
    }

    const int OFFSET = 4;

    if (line_p->content == NULL)
    {
        line_p->alignment = max_content_size + OFFSET;
        return;
    }

    line_p->alignment = max_content_size + OFFSET - strlen(line_p->content);
}