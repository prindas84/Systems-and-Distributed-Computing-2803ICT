#include "include.h"

int main()
{
    // Initialise the user input variable.
    char user_input[INPUT_SIZE];

    // Begin the command line loop until the user types "quit"
    while (true)
    {
        // Request user command and trim.
        printf("Enter Command: ");
        fgets(user_input, INPUT_SIZE, stdin);
        user_input[strlen(user_input) - 1] = '\0';
        
        // If the command is "quit", break the loop.
        if (strcmp(user_input, "quit") == 0)
        {
            break;
        }

        // If the command is not "quit", process the command.
        else
        {
            process_command(user_input);
        }
    };
    return 0;
}