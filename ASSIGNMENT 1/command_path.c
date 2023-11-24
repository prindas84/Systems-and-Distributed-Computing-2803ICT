#include "include.h"

void command_path()
{
    // Initialise an empty array to store the current working directory.
    char cwd[DIRECTORY_SIZE];

    // Extract the current working directory.
    getcwd(cwd, sizeof(cwd));

    // If there are no errors, print the current working directory. 
    if (cwd != NULL)
    {
        printf("%s \n", cwd);
    }
    else
    {
        printf("ERROR: COULD NOT PRINT THE CURRENT WORKING DIRECTORY\n");
        return;
    }
}
