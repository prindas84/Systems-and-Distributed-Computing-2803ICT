#include "include.h"

// This file contains all general functions used within the program. It does not include commands.

int argument_count(char* user_input)
{
    // Initialise the word count to 1, then loop through the original input incrementing count on ' '.
    int count = 1;

    // Check that the last character is not a space. Remove it if it is.
    if (user_input[strlen(user_input) - 1] == ' ')
    {
        user_input[strlen(user_input) - 1] = '\0';
    }

    // Iterate over the string and count the spaces to find the number of words.
    for (int i = 0; user_input[i] != '\0'; i++)
    {
        if (user_input[i] == ' ')
        {
            count++;
        }
    }
    return count;
}


void clear_input_buffer()
{
    // Continue to read getchar into "ch" until the new line character is the last character left in the input buffer.
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
    return;
}


void copy_file(char* file_name, char* directory_name)
{
    // Initialise a buffer for the command.
    char command[COMMAND_SIZE];

    // Format the command into the command variable. Store Location, Size, Format (PrintF).
    snprintf(command, sizeof(command), "cp %s %s/", file_name, directory_name);

    // Process the command.
    if (system(command) != 0) {
        printf("ERROR: COULD NOT FIND OR COPY THE FILE %s.\n", file_name);
    }
    return;
}


void make_directory(char* directory_name, int permissions)
{
    // Call mkdir() to create a new directory with the permissions set.
    if (mkdir(directory_name, permissions) != 0) 
    {
        printf("ERROR: COULD NOT REMOVE THE DIRECTORY.\n");
    }
    return;
}


void process_command(char* user_input)
{
    // Process the commands.
    if (strcmp(user_input, "time") == 0)
    {
        command_time();
    }
    else if (strcmp(user_input, "path") == 0)
    {
        command_path();
    }
    else if (strcmp(user_input, "sys") == 0)
    {
        command_sys();
    }
    else if (strncmp(user_input, "calc ", 5) == 0) 
    {
        // Count how many arguments there are in the command.
        int word_count = argument_count(user_input);

        // If there is an expression attached, pass the information to the command_calc function.
        if (word_count > 1)
        {
            // Split the arguments into an array of strings.
            char** split_input = split_string(user_input, word_count);
            command_calc(split_input, word_count);
            free(split_input);
        }
        else
        {
            printf("ERROR: MISSING PREFIX EXPRESSION\nUSAGE: 'calc + - 9 8 10'\n");
        }
    }
    else if (strncmp(user_input, "put ", 4) == 0) 
    {
        // Count how many arguments there are in the command.
        int word_count = argument_count(user_input);

        // If there are filenames attached, pass the information to the command_put function.
        if (word_count > 2)
        {
            // Split the arguments into an array of strings.
            char** split_input = split_string(user_input, word_count);

            // If the user is using the -f command...
            if (strcmp(split_input[word_count - 1], "-f") == 0)
            {
                if (word_count > 3)
                {
                    command_put(split_input, word_count, true);
                }
                else
                {
                    printf("ERROR: MISSING FILENAME\nUSAGE ONE: 'put C:/path/to/directory_name C:/path/to/example.txt [-f]'\nUSAGE TWO: 'put directory_name  filename.txt [-f]'\n");
                }
            }

            // If the user is not using the -f command...
            else
            {
                command_put(split_input, word_count, false);
            }
            free(split_input);
        }
        else
        {
            printf("ERROR: MISSING FILENAME\nUSAGE ONE: 'put C:/path/to/directory_name C:/path/to/example.txt [-f]'\nUSAGE TWO: 'put directory_name  filename.txt [-f]'\n");
        }
    }
    else if (strncmp(user_input, "get ", 4) == 0) 
    {
        // Count how many arguments there are in the command.
        int word_count = argument_count(user_input);

        // If there is a filename attached, pass the information to the command_get function.
        if (word_count > 1)
        {
            // Split the line at the first " " and use the remainin string as the filename.
            char* file_name = strchr(user_input, ' ');

            // If there is no error, remove the " " from the start of the string.
            if (file_name != NULL)
            {
                file_name += 1;
                command_get(file_name);
            }
        }
        else
        {
            printf("ERROR: MISSING FILENAME\nUSAGE ONE: 'get C:/path/to/example.txt'\nUSAGE TWO: 'get example.txt'\n");
        }
    }
    // Cover all other alternative incorrect input options.
    else
    {
        printf("ERROR - PLEASE TRY AGAIN: COMMANDS ARE CASE AND WHITESPACE SENSITIVE\n");
    }
    return;
}


void remove_directory(char* directory_name)
{
    // Initialise a buffer for the command.
    char command[COMMAND_SIZE];

    // Format the command into the command variable. Store Location, Size, Format (PrintF).
    snprintf(command, sizeof(command), "rm -r %s", directory_name);

    // Process the command.
    if (system(command) != 0) {
        printf("ERROR: COULD NOT REMOVE THE DIRECTORY.\n");
    }
    return;
}


char** split_string(char* user_input, int word_count)
{
    // Initialise the array of pointers to create an string array of dynamic length arguments.
    char** split_input = (char**)malloc(word_count * sizeof(char*));
    if (split_input == NULL)
    {
        printf("ERROR: UNABLE TO SPLIT THE USER INPUT");
        return split_input;
    }

    // Loop through the original string. Split the string into arguments on " " and save as a "word" variable. 
    // Assign enough memory to the array pointer and copy the word into the array.
    int j = 0;
    char* argument = strtok(user_input, " ");

    // While there is an argument to process, allocate memory, and copy the string to the array.
    while (argument != NULL)
    {
        split_input[j] = (char*)malloc(strlen(argument) + 1);
        if (split_input[j] == NULL)
        {
            printf("ERROR: UNABLE TO SPLIT THE USER INPUT");
            return split_input;
        }
        strcpy(split_input[j], argument);
        argument = strtok(NULL, " ");
        j++;
    }
    return split_input;
}