#include "include.h"

void command_get(char* file_name)
{
    // Open the file name or file path and check it was opened correctly.
    FILE* input_file = fopen(file_name, "r");
    if (input_file == NULL)
    {      
        printf("ERROR: COULD NOT OPEN THE FILENAME PROVIDED\nUSAGE ONE: 'get C:/path/to/example.txt'\nUSAGE TWO: ''get example.txt'\n");
        return;
    }
    else
    {
        bool repeat = true;
        // While the function is set to true, loop through the file at 40 lines per iteration.
        while (repeat) 
        {
            char str[LINE_SIZE];
            for (int i = 0; i < 40; i++)
            {
                // Read each line and print it to the screen. If the end of file is reached, stop repeat and break loop.
                if((fgets(str, LINE_SIZE, input_file)) != NULL)
                {
                    printf("%s", str);
                }
                else
                {
                    printf("\nEND OF FILE...\n");
                    repeat = false;
                    break;
                }
            }
            // If the function is set to false, break the loop. Otherwise, prompt user to continue or quit.
            if (!repeat)
            {
                break;
            }
            else
            {
                printf("\nPRESS 'Q + ENTER' TO QUIT.\nPRESS ANY OTHER KEY + ENTER TO CONTINUE.\n");
                char ch = getchar();
                // Clear the input buffer to stop it from affecting future prompt requests.
                clear_input_buffer();
                if (ch == 'q' || ch == 'Q')
                {
                    break;
                }
            }
        }
        fclose(input_file);
    }    
    return;
}