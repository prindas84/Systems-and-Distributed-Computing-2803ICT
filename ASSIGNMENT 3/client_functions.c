#include "include.h"

void* client_connected(void* args)
{
    // Cast the args to the correct type.
    struct ThreadArgsClient* targs = (struct ThreadArgsClient*) args;

    // Set the connection to true.
    while (true)
    {
        if (targs->shared_data->client_flag != NOT_READY)
        {
            int temp = targs->shared_data->client_flag;
            targs->shared_data->client_flag = NOT_READY;
            targs->shared_data->connected = true;
            targs->shared_data->client_flag = temp;
            break;
        }
        // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
        int n = generate_random_number(1, 100000);
        usleep(n);
    }

    // Print the user intructions.
    printf("\n------ INSTRUCTIONS ------\n");
    printf("1. TYPE '0' TO RUN TEST MODE (ONCE ONLY)\n");
    printf("2. TYPE 'q' OR 'Q' TO QUIT (AT ANY TIME)\n");
    printf("3. ENTER A 32-BIT INTEGER (AS REQUIRED)\n");

    while(targs->shared_data->connected)
    {
        // Initialise the number buffer.
        char number[MAX_LENGTH];

        // Create a flag to print the system busy statement only once.
        bool max_input = false;

        // Prompt the user to input a number. Loop and check for correct usage.
        while(true)
        {
            // If the system has reached its maximum process limit, pause until another process becomes available.
            if (targs->shared_data->process_count >= MAX_PROCESS)
            {
                // If the system busy message has not been printed, print it and change the flag.
                if (!max_input)
                {
                    printf("\nSYSTEM BUSY: Please Wait...\n");
                    max_input = true;
                }
                sleep(2);
            }

            // If the system was previously busy and is now ready, print the ready message and reset the flag.
            if (max_input)
            {
                printf("\nSYSTEM READY: Please Continue...\nENTER A 32-BIT INTEGER (AS REQUIRED)\n");
                clear_buffer();
                max_input = false;
            }

            // Scan for users input of a 32-bit integer.
            scanf("%s", number);

            // If the user enters 'q', exit the program.
            if (strcmp(number, "q") == 0 || strcmp(number, "Q") == 0)
            {
                // Print disconnection message and apply correct settings.
                printf("\nPlease Wait - Disconnecting you now...\n");

                while (true)
                {
                    if (targs->shared_data->client_flag != NOT_READY)
                    {
                        int temp = targs->shared_data->client_flag;
                        targs->shared_data->client_flag = NOT_READY;
                        targs->shared_data->connected = false;
                        targs->shared_data->client_flag = temp;
                        break;
                    }
                    // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
                    int n = generate_random_number(1, 100000);
                    usleep(n);
                }

                // Notify the server that new data is available.
                sem_post(targs->sem_server);

                return NULL;
            }

            // If the user enters '0', run test mode.
            else if (strcmp(number, "0") == 0 && !targs->shared_data->standard_mode)
            {
                // Print test mode message and apply correct settings.
                printf("\nPlease Wait - Running Test Mode...\n");
                while (true)
                {
                    if (targs->shared_data->client_flag != NOT_READY)
                    {
                        int temp = targs->shared_data->client_flag;
                        targs->shared_data->client_flag = NOT_READY;
                        targs->shared_data->test_mode = true;
                        targs->shared_data->client_flag = temp;
                        break;
                    }
                    // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
                    int n = generate_random_number(1, 100000);
                    usleep(n);
                }                

                // Notify the server that new data is available.
                sem_post(targs->sem_server);

                while (targs->shared_data->test_completed < TEST_CASES * TEST_THREADS)
                {
                    sem_wait(targs->sem_client);
                    print_array(targs->shared_data->test_numbers, TEST_THREADS);
                }

                printf("\nTEST MODE COMPLETE: Disconnecting you now...\n");

                return NULL;
            }

            // Check the number is a valid 32-bit number.
            else if (valid_input(number))
            {
                // If the client has submitted at least one valid input, set to standard mode so test mode can't run.
                while (true)
                {
                    if (targs->shared_data->client_flag != NOT_READY)
                    {
                        int temp = targs->shared_data->client_flag;
                        targs->shared_data->client_flag = NOT_READY;
                        targs->shared_data->standard_mode = true;
                        targs->shared_data->client_flag = temp;
                        break;
                    }
                    // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
                    int n = generate_random_number(1, 100000);
                    usleep(n);
                }                
                break;
            }

            // If the number is not valid, print an error message and repeat the loop.
            else
            {
                printf("\nERROR: USAGE - INTEGER %lu <= N <= %lu\n", MIN_32BIT, MAX_32BIT);
            }
        }        

        // If the maximum number of processes has not been reached, send the number to the server.
        if (targs->shared_data->process_count < MAX_PROCESS)
        {
            // Get the next index from the index queue to store the data for this process, then increment the process count.
            pop_request_queue(targs->shared_data);

            // Convert the string to an unsigned long integer and store in shared memory data array. 10 is the base (base 10).
            char *endptr;
            while (true)
            {
                if (targs->shared_data->client_flag != NOT_READY)
                {
                    int temp = targs->shared_data->client_flag;
                    targs->shared_data->client_flag = NOT_READY;
                    targs->shared_data->numbers[targs->shared_data->current_index] = strtoul(number, &endptr, 10);
                    targs->shared_data->client_flag = temp;
                    break;
                }
                // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
                int n = generate_random_number(1, 100000);
                usleep(n);
            }            

            // Notify the server that new data is available.
            sem_post(targs->sem_server);

            // Wait for the server to signal that it's processed the data.
            sem_wait(targs->sem_client);
        }
    }
    
    return NULL;
}

void* print_client_processes(void* args)
{
    // Cast the arguments to the correct size.
    struct ThreadArgsClient* targs = (struct ThreadArgsClient*) args;
    int percentage;

    // Check the process flags and print the percentage of each process accordingly.
    while(true)
    {
        // Only print if there are processes to print and the client is connected.
        if (targs->shared_data->process_count > 0 && targs->shared_data->connected)
        {
            // Print format type 1.
            if (PRINT_FORMAT == 1)
            {
                // Print each process that is active.
                printf("Progress: ");
                for (int i = 0; i < MAX_PROCESS; i++)
                {
                    if (targs->shared_data->server_flag[i])
                    {
                        // Calculate the percentage to 5%.
                        int raw_percentage = (targs->shared_data->progress_percentage[i] * 100) / MAX_BITS;
                        if (raw_percentage == 100)
                        {
                            percentage = 100;
                        } 
                        else 
                        {
                            percentage = raw_percentage - (raw_percentage % 5);
                        }
                        printf("Query %d: %d%% ", i + 1, percentage);
                        fflush(stdout);

                        // Terminate the process once it is complete.
                        if (percentage == 100)
                        {
                            terminate_process(targs->shared_data, i);
                        }
                    }
                }
                printf("\n\n");
            }

            // Print format type 2.
            else if (PRINT_FORMAT == 2)
            {
                // Print each process that is active.
                printf("Progress: ");
                for (int i = 0; i < MAX_PROCESS; i++)
                {
                    if (targs->shared_data->server_flag[i])
                    {
                        // Calculate the percentage to 5%.
                        int raw_percentage = (targs->shared_data->progress_percentage[i] * 100) / MAX_BITS;
                        if (raw_percentage == 100)
                        {
                            percentage = 100;
                        } 
                        else 
                        {
                            percentage = raw_percentage - (raw_percentage % 5);
                        }

                        // Number of bars to represent the percentage. 10% per bar.
                        int bars = percentage / 10;   
                        printf("Q%d:%d%% ", i + 1, percentage);
                        for (int j = 0; j < bars; j++)
                        {
                            printf("▓");
                        }
                        for (int j = bars; j < 10; j++)
                        {
                            printf("_");
                        }
                        printf("| ");
                        fflush(stdout);

                        // Terminate the process once it is complete.
                        if (percentage == 100)
                        {
                            terminate_process(targs->shared_data, i);
                        }
                    }
                }
                printf("\n\n");
            }
        }
        usleep(500000);
    }

    return NULL;
}


void terminate_process(struct Memory* shared_data, int i)
{
    // Reset the server flag so the threads stop processing. Push the current process index back to the queue to reuse.
    shared_data->server_flag[i] = false;
    push_request_queue(shared_data, i);

    return;
}

    