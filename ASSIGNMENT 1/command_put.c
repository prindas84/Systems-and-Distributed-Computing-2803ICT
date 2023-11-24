#include "include.h"

void command_put(char** split_input, int word_count, bool force)
{
    // Extract the directory name from the input array and open the directory.
    char* directory = split_input[1];
    DIR* dir = opendir(directory);

    // If the directory already exists, provide error message or reset the directory when -f is requested.
    if (dir != NULL)
    {
        if (force)
        {
            remove_directory(directory);
            make_directory(directory, 0755);
        }
        else
        {
            printf("ERROR: THIS DIRECTORY ALREADY EXISTS. TRY -f TO FORCE THE COMMAND.\n");
            return;
        }
    }
    // If the directory can't be opened because it does not exist, create the directory.
    else
    {
        make_directory(directory, 0755);   
    }

    // Position the index pointers to the first and last filename in the array.
    int file_current_ptr = 2;
    int file_end_ptr;
    if (force)
    {
        file_end_ptr = word_count - 2;
    }
    else
    {
        file_end_ptr = word_count - 1;
    }

    // Iterate over the filenames and copy them to the new directory.
    while (file_current_ptr <= file_end_ptr)
    {
        copy_file(split_input[file_current_ptr], directory);
        file_current_ptr++;
    }
    return;
}

