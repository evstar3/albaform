/* albaform.c

A formatter for albaCore assembly files.

Author: Evan Day
*/

#include "file_io.h"

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