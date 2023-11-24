#include "include.h"

int main() {
    
    // Declare a file descriptor for shared memory, a pointer to shared data, and semaphore pointers.
    int fd;
    struct Memory* shared_data;
    sem_t *sem_client, *sem_server;

    // Open semaphores for reading and writing.
    sem_client = sem_open(SEM_CLIENT, O_RDWR);
    sem_server = sem_open(SEM_SERVER, O_RDWR);

    // Open the shared memory segment with read and write permissions.
    fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1)
    {
        perror("ERROR: Unable to open shared memory (client). Check server configuration.");
        exit(1);
    }

    // Map the shared memory segment to a pointer in the process.
    shared_data = mmap(NULL, sizeof(struct Memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Initialise the client threads and the required arguments.
    pthread_t thread_1, thread_2;
    struct ThreadArgsClient args;
    args.shared_data = shared_data;
    args.sem_client = sem_client;
    args.sem_server = sem_server;

    // Create a thread to handle the client inputs.
    int status_1 = pthread_create(&thread_1, NULL, client_connected, &args);
    if (status_1 != 0)
    {
        fprintf(stderr, "Failed to create thread (client 1).\n");
        exit(1);
    }
    
    // Pause until the first thread has properly connected.
    while(!shared_data->connected)
    {
        usleep(500);
    }

    // Create a thread to handle the client printing.
    int status_2 = pthread_create(&thread_2, NULL, print_client_processes, &args);
    if (status_2 != 0)
    {
        fprintf(stderr, "Failed to create thread (client 2).\n");
        exit(1);
    }

    // Pause until the the client has disconnected.
    while(shared_data->connected)
    {
        sleep(2);
    }

    // Close the file descriptor before exiting.
    close(fd);  

    // Close the semaphores.
    sem_close(sem_client);
    sem_close(sem_server);

    return 0;
}
