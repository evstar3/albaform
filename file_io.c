#include "file_io.h"

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
            break;
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

