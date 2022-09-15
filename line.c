#include "line.h"

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
        // stop copying when comment is reached
        if (*r == '/' && *(r + 1) == '/')
            break;

        *w = *r;
        w++;
        r++;
    }
    *w = 0;

    // move past comment indicator "//"
    r += 2; 

    // copy comment
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

        // move to the end
        r = parsed.comment;
        while (*r)
            r++;

        // start writing one after the read pointer
        w = r + 1;

        // copy all of the comment
        while (r >= parsed.comment)
        {
            *w = *r;
            r--;
            w--;
        }

        // the final character is the space we want to add
        *w = ' ';
    }

    free(line);

    return parsed;
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

bool isLabel(char *line)
{
    // do not indent .text, .data, etc.
    if (line[0] == '.')
        return true;

    // check for colon character
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