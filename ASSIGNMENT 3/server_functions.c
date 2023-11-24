#include "include.h"

void create_server_threads(struct Memory* shared_data)
{
    // Initialise the server threads and the required arguments.
    pthread_t threads[MAX_PROCESS][MAX_BITS];
    struct ThreadArgsServer args[MAX_PROCESS][MAX_BITS];

    // Loop through and populate the thread arguments, then launch each thread.
    for (int i = 0; i < MAX_PROCESS; i++)
    {
        for (int j = 0; j < MAX_BITS; j++)
        {
            args[i][j].shared_data = shared_data;
            args[i][j].current_index = i;
            args[i][j].number = j;
            int status = pthread_create(&threads[i][j], NULL, server_thread, &args[i][j]);
            if (status != 0)
            {
                fprintf(stderr, "Failed to create thread \n");
                exit(1);
            }
        }
    }

    // Pause until the threads have all completed.
    while (shared_data->thread_count != (MAX_PROCESS * MAX_BITS))
    {
        usleep(500);
    }
    
    // Print thread completion message.
    printf("\nALL SERVER THREADS HAVE BEEN CREATED\n");

    return;
}

void* server_thread(void* args)
{
    // Cast the args to the correct type.
    struct ThreadArgsServer* targs = (struct ThreadArgsServer*) args;
    int number = targs->number;
    int current_index = targs->current_index;

    // Declare a variableS for the factorising function.
    unsigned long* factors;
    unsigned long n;
    int num_factors;

    // Increment the thread count to confirm they have all been completed.
    while (true)
    {
        if (targs->shared_data->client_flag != NOT_READY)
        {
            int temp = targs->shared_data->client_flag;
            targs->shared_data->client_flag = NOT_READY;
            targs->shared_data->thread_count++;
            targs->shared_data->client_flag = temp;
            break;
        }
        // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
        int n = generate_random_number(1, 100000);
        usleep(n);
    }

    // Run the thread continuously polling the flag array to know when to begin.
    while(true)
    {
        // If the client is connected, and the process server flag has been triggered...
        if (targs->shared_data->connected && targs->shared_data->server_flag[current_index] && !targs->shared_data->progress_percentage_flag[current_index][number])
        {
            // Factorising the number provided.
            n = targs->shared_data->rotated_numbers[current_index][number];
            factors = trial_division_factorise(n, &num_factors);
            free(factors);

            // Increment the total for this process to calculate the percentage.
            targs->shared_data->progress_percentage[current_index]++;

            if (targs->shared_data->progress_percentage[current_index] == MAX_BITS)
            {
                printf("\nPROCESS %d HAS IS NOW COMPLETE\n", current_index + 1);
            }
            
            // Trigger the flag to mark it as completed.
            while (true)
            {
                if (targs->shared_data->client_flag != NOT_READY)
                {
                    int temp = targs->shared_data->client_flag;
                    targs->shared_data->client_flag = NOT_READY;
                    targs->shared_data->progress_percentage_flag[current_index][number] = true;
                    targs->shared_data->client_flag = temp;
                    break;
                }
                // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
                int n = generate_random_number(1, 100000);
                usleep(n);
            }
        }
        else
        {
            sleep(5);
        }    
    }

    return NULL;
}

void server_connected(struct Memory* shared_data, sem_t *sem_client, sem_t *sem_server)
{
    // Create the server threads once the server has been launched.
    create_server_threads(shared_data);

    // Loop to process data from the client.
    while(true)
    {
        // If the system is in test mode...
        if (shared_data->connected && shared_data->test_mode)
        {
            printf("\nRUNNING TEST MODE...\n");

            shared_data->test_completed = 0;
            int test_count = TEST_CASES * TEST_THREADS;
            
            // Initialise the server threads and the required arguments.
            pthread_t threads[TEST_CASES][TEST_THREADS];
            struct ThreadArgsTest args[TEST_CASES][TEST_THREADS];

            // Loop through and populate the thread arguments, then launch each thread.
            for (int i = 0; i < TEST_CASES; i++)
            {
                for (int j = 0; j < TEST_THREADS; j++)
                {
                    args[i][j].shared_data = shared_data;
                    args[i][j].case_number = i;
                    args[i][j].thread_number = j;
                    args[i][j].sem_client = sem_client;
                    args[i][j].sem_server = sem_server;
                    int status = pthread_create(&threads[i][j], NULL, test_mode, &args[i][j]);
                    if (status != 0)
                    {
                        fprintf(stderr, "Failed to create thread \n");
                        exit(1);
                    }
                }
            }

            // Loop until all test threads have been created, then print message.
            while (true)
            {
                if (shared_data->test_completed == test_count)
                {
                    printf("\nALL TEST THREADS HAVE BEEN CREATED\n");
                    shared_data->connected = false;
                    break;
                }
                sleep(1);
            }
            
            return;
        }

        // If the client has disconnected...
        else if (!shared_data->connected)
        {
            return;
        }

        // If there are processes available and the system is not busy...
        else if (shared_data->connected && shared_data->process_count < MAX_PROCESS)
        {
            // Wait for the client to indicate that new data has been written to the shared memory.
            sem_wait(sem_server);

            // If the client has written data to the shared memory...
            if (shared_data->client_flag == FILLED)
            {
                // Print the current number.
                printf("\nRUNNING PROCESS: %d\tINPUT NUMBER: %lu\n", shared_data->current_index + 1, shared_data->numbers[shared_data->current_index]);

                // Notify the client that the server has processed the data and it can proceed.
                sem_post(sem_client);
                int number = shared_data->numbers[shared_data->current_index];
                int index = shared_data->current_index;
                while (true)
                {
                    if (shared_data->client_flag != NOT_READY)
                    {
                        shared_data->client_flag = TAKEN;
                        break;
                    }
                    // Generate a random number of micro-seconds to sleep for, so process don't continuously bounce off each other on the same timing loop.
                    int n = generate_random_number(1, 100000);
                    usleep(n);
                }
                
                // If the maximum input count has been reached, inform the user.
                if (shared_data->process_count >= MAX_PROCESS)
                {
                    printf("\nSYSTEM BUSY: Maximum Inputs Reached. Please Wait...\n");
                }
                trigger_process(shared_data, index, number);
            }
        }
    }

    // Don't exit the function while the client is still connected.
    while(shared_data->connected)
    {
        sleep(1);
    }

    return;
}

void trigger_process(struct Memory* shared_data, int index, int number)
{
    // Rotate the number provided by the client and store the array in shared memory.
    shared_data->rotated_numbers[index][0] = number;
    int* binary_number = (int*)calloc(MAX_BITS, sizeof(int)); 
    for (int i = 1; i < MAX_BITS; i++)
    {
        decimal_to_binary(binary_number, shared_data->rotated_numbers[index][i - 1]);
        rotate_binary(binary_number);
        shared_data->rotated_numbers[index][i] = binary_to_decimal(binary_number);
    }
    free(binary_number);

    // Reset the progress for the current process before starting the threads.
    shared_data->progress_percentage[index] = 0;

    // Reset the progress flags for the current index before starting the threads.
    for (int i = 0; i < MAX_BITS; i++)
    {
        shared_data->progress_percentage_flag[index][i] = false;
    }

    // Trigger the flag to run the factorising threads for this process.
    shared_data->server_flag[index] = true;

}

    