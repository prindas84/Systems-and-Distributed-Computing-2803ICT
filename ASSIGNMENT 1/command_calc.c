#include "include.h"

void command_calc(char** split_input, int word_count)
{
    // Initialise a stack array with a maximum size of the total equation, and a index pointer to keep track of the top position.
    // I used this simple method to save time on building a full stack structure which was not nessecary for this problem.
    int* stack_array = (int*)calloc(word_count, sizeof(int));
    int stack_ptr = -1;
    int prefix_ptr = word_count - 1;

    while (prefix_ptr > 0)
    {
        // Initialise variables for calculations to 0.
        int x = 0;
        int y = 0;
        int sum = 0;

        // Add all digits to the stack until an operator is found.
        while ((strcmp(split_input[prefix_ptr], "+") != 0) && (strcmp(split_input[prefix_ptr], "-") != 0))
        {
            // Add each digit to the stack and adjust the pointers.
            stack_array[++stack_ptr] = atoi(split_input[prefix_ptr]);
            prefix_ptr--;
            if (prefix_ptr == 0)
            {
                break;
            }
        }

        // Pop the last two digits from the stack into X and Y, reset them and reposition the stack pointer.
        x = stack_array[stack_ptr];
        y = stack_array[stack_ptr - 1];

        // Make the calculation according to the operand.
        if (strcmp(split_input[prefix_ptr], "+") == 0)
        {
            sum = x + y;
        }
        else
        {
            sum = x - y;  
        }

        // Remove X and Y from the stack and add the sum of each to the stack.
        stack_array[stack_ptr] = 0;
        stack_array[--stack_ptr] = sum;
        prefix_ptr--;
    }
    printf("ANSWER: %i\n", stack_array[stack_ptr]);
    free(stack_array);
    return;
}