#include "include.h"

int main() {

    // Declare a file descriptor for shared memory, a pointer to the shared data, and semaphore pointers.
    int fd;
    struct Memory* shared_data;
    sem_t *sem_client, *sem_server;

    // Declare a variable for the factorising function.
    int num_factors;

    // Delete any pre-existing semaphores to ensure a fresh start.
    sem_unlink(SEM_CLIENT);
    sem_unlink(SEM_SERVER);

    // Create and initialize the semaphores with a value of 0 (locked).
    sem_client = sem_open(SEM_CLIENT, O_CREAT, 0666, 0);
    sem_server = sem_open(SEM_SERVER, O_CREAT, 0666, 0);

    // Create a shared memory segment or open it if it already exists.
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("\nERROR: Unable to open the shared memory (server).\n");
        exit(1);
    }
    
    // Adjust the size of the shared memory segment to match the size of the Memory structure.
    ftruncate(fd, sizeof(struct Memory));

    // Map the shared memory segment into the server's address space.
    shared_data = mmap(NULL, sizeof(struct Memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Initialise the standard Memory struct variables.
    shared_data->client_flag = TAKEN;
    shared_data->connected = true;
    shared_data->current_index = -1;
    shared_data->end_queue = MAX_PROCESS - 1;
    shared_data->process_count = 0;
    shared_data->standard_mode = false;
    shared_data->test_mode = false;
    shared_data->thread_count = 0;
    memset(shared_data->numbers, 0, sizeof(shared_data->numbers));
    memset(shared_data->progress_percentage, 0, sizeof(shared_data->progress_percentage));
    
    // Initialise Memory struct variables that require a loop.
    for (int i = 0; i < MAX_PROCESS; i++)
    {
        shared_data->available_process_queue[i] = i;
        shared_data->server_flag[i] = false;
        memset(shared_data->rotated_numbers[i], 0, sizeof(shared_data->rotated_numbers[i]));
        for (int j = 0; j < MAX_BITS; j++)
        {
            shared_data->progress_percentage_flag[i][j] = false;
        }
    }

    // Run the connected server...
    server_connected(shared_data, sem_client, sem_server);

    // Print disconnection message.
    printf("\nClient Disconnected - Shutting Down...\n");

    // Unmap the shared memory, unlink shared memory and semaphores, and close everything.
    munmap(shared_data, sizeof(struct Memory));
    shm_unlink(SHM_NAME);
    sem_close(sem_client);
    sem_close(sem_server);
    sem_unlink(SEM_CLIENT);
    sem_unlink(SEM_SERVER);

    return 0;
}
