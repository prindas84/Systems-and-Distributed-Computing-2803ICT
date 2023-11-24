#include "include.h"

unsigned long binary_to_decimal(int* binary_array)
{
    // This function converts an integer array representing a binary number into a decimal number.
    unsigned long n = 0;
    for (int i = 0; i < MAX_BITS; i++)
    {
        n = (n << 1) | binary_array[i];
    }

    return n;
}


void clear_buffer()
{
    /*  REFERENCE: This function was written by chatGPT for the purpose of clearing the entire buffer
        in the case the client user enters multiple inputs while the process is in a holding pattern. 
        
        I have written similar functions previously, however, in the instance of saving time, I thought 
        it best to reference this instead of writing it myself, as it was not specifically required in the document.   */

    char ch;
    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = 0;
    tv.tv_usec = 0;  // Set the wait time to 0 for non-blocking

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    while (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
        read(STDIN_FILENO, &ch, 1);  // Read and discard the character
        FD_SET(STDIN_FILENO, &readfds);  // Reset the fd set for the next iteration
    }

    return;
}


void decimal_to_binary(int* binary_array, unsigned long n)
{
    // This function converts a decimal number into an integer array representing a binary number.
    for (int i = MAX_BITS - 1; i >= 0; i--)
    {
        binary_array[i] = n & 1;
        n >>= 1;
    }

    return;
}


int generate_random_number(int min, int max)
{
    // This function generates a random number between the min and max arguments.
    srand((unsigned) time(NULL));

    return min + (rand() % (max + 1 - min));
}

void generate_test_numbers(int* arr, int n)
{
    arr[0] = n * 10;
    for (int i = 1; i < 10; i++)
    {
        arr[i] = (n * 10) + i; 
    }

    return;
}


void pop_request_queue(struct Memory* shared_data)
{
    // If the data is blocked by another process, pause until it is ready.
    while (shared_data->client_flag == NOT_READY)
    {
        // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
        int n = generate_random_number(1, 100000);
        usleep(n);
    }

    // Block the process while using the data.
    shared_data->client_flag = NOT_READY;

    // Get the next index from the queue to store the data for the next process.
    shared_data->current_index = shared_data->available_process_queue[0];

    // Reorder the queue.
    for (int i = 1; i <= shared_data->end_queue; i++)
    {
        shared_data->available_process_queue[i - 1] = shared_data->available_process_queue[i];
    }

    // Decrement the end position of the queue
    shared_data->end_queue--;

    // Increment the process count.
    shared_data->process_count++;

    // Unblock the process while using the data.
    shared_data->client_flag = FILLED;

    return;
}


void print_array(int* binary_array, int n)
{
    // This function prints an array of integers.
    for (int i = 0; i < n; i++)
    {
        printf("%d ", binary_array[i]);
    }
    printf("\n");

    return;
}


void print_array_long(unsigned long* num_array, int n)
{
    // This function prints an array of unsigned long intergers.
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        printf("%lu", num_array[i]);
    }
    printf("\n");

    return;
}

void push_request_queue(struct Memory* shared_data, int i)
{
    // If the data is blocked by another process, pause until it is ready.
    while (shared_data->client_flag == NOT_READY)
    {
        // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
        int n = generate_random_number(1, 100000);
        usleep(n);
    }

    // Record what the previous flag was set too.
    int temp = shared_data->client_flag;

    // Block the process while using the data.
    shared_data->client_flag = NOT_READY;

    // Push the current index to the end of the queue.
    shared_data->available_process_queue[shared_data->end_queue + 1] = i;

    // Increment the end position of the queue.
    shared_data->end_queue++;

    // Decrement the process count.
    shared_data->process_count--;

    // Reset the 

    // Unblock the process while using the data.
    shared_data->client_flag = temp;

    return;
}


void rotate_binary(int* binary_array)
{
    // This function will rotate the digits in an array of integers one position to the right.
    int temp = binary_array[MAX_BITS - 1];

    for (int i = MAX_BITS - 1; i > 0; i--)
    {
        binary_array[i] = binary_array[i - 1];
    }

    binary_array[0] = temp;

    return;
}


unsigned long* trial_division_factorise(unsigned long number, int *size)
{
    /*  REFERENCE: This function was converted to C language from the example provided at the following URL:
    https://en.wikipedia.org/wiki/Trial_division   */

    unsigned long* factors = (unsigned long *)malloc(sizeof(unsigned long) * number);   // Allocate an array for the maximum number if possible factors.
    unsigned long divisor = 2;                                                          // Start the divisor as the lowest possible option.
    *size = 0;                                                                          // Initialize the size of the factors list

    // Find the factors.
    while (number % 2 == 0) {
        factors[*size] = 2;
        (*size)++;
        number /= 2;
    }

    divisor = 3;
    while (divisor * divisor <= number) {
        if (number % divisor == 0) {
            factors[*size] = divisor;
            (*size)++;
            number /= divisor;
        } else {
            divisor += 2;
        }
    }

    if (number != 1) {
        factors[*size] = number;
        (*size)++;
    }

    factors = (unsigned long *)realloc(factors, (*size) * sizeof(unsigned long));
     
    return factors;
}


void* test_mode(void* args)
{
    // Cast the args to the correct type.
    struct ThreadArgsTest* targs = (struct ThreadArgsTest*) args;
    int n = targs->case_number * 10 + targs->thread_number;

    // Lock the shared data.
    pthread_mutex_lock(&targs->shared_data->test_mutex);
        
    // Generate the test numbers.
    generate_test_numbers(targs->shared_data->test_numbers, n);

    // Notify the client to print the shared numbers.
    sem_post(targs->sem_client);

    // Increment the completed count.
    targs->shared_data->test_completed++;
    
    // Unlock the shared data.
    pthread_mutex_unlock(&targs->shared_data->test_mutex);
    
    return NULL;
}


bool valid_input(char* str)
{
    // Iterate through each character in the string to check if it's a digit. Return false if a non-digit character is found.
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }

    // Convert the string to an unsigned long integer.
    char *endptr;
    unsigned long n = strtoul(str, &endptr, 10);

    // Check for successful conversion.
    if (*endptr != '\0')
    {
        return false;
    }

    // Check if it's within the desired range.
    if (n < MIN_32BIT || n > MAX_32BIT)
    {
        return false;
    }

    return true;
}


