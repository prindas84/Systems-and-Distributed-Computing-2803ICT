#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define SHM_NAME "/shared_memory"
#define SEM_CLIENT "/sem_client"
#define SEM_SERVER "/sem_server"
#define MAX_BITS 32
#define MAX_32BIT 4294967295
#define MIN_32BIT 1
#define MAX_PROCESS 10
#define MAX_LENGTH 64
#define PRINT_FORMAT 2
#define TEST_CASES 3
#define TEST_THREADS 10

// Client state enumeration for readability.
enum client_states { NOT_READY, FILLED, TAKEN };

struct Memory
{
    int available_process_queue[MAX_PROCESS];               // A queue of index numbers to corrospond with the process rotation.
    int client_flag;                                        // Monitors the status of the shared memory in regard to manipulating data.
    bool connected;                                         // Triggers when the client first connects to the server.
    int current_index;                                      // The index number which corrosponds to the current process.
    int end_queue;                                          // The index number of the last position in the queue.
    unsigned long numbers[MAX_PROCESS];                     // An array of numbers which have been inputted by the client for each process.
    unsigned long rotated_numbers[MAX_PROCESS][MAX_BITS];   // All rotations of the number inputted by the client for each process.
    int process_count;                                      // The current number of processes running.
    int progress_percentage[MAX_PROCESS];                   // An array to monitor the progress for the factorising of each process. 
    bool progress_percentage_flag[MAX_PROCESS][MAX_BITS];   // Triggers when each thread has completed it's task and incremented the percentage.
    bool server_flag[MAX_PROCESS];                          // Triggers when a process is reading to be read by the threads. 
    bool standard_mode;                                     // Triggers if the user proceeds in standard mode.
    int test_completed;                                     // A count of how many test threads have been completed.
    bool test_mode;                                         // Triggers if the user proceeds in test mode.
    int test_numbers[TEST_THREADS];                         // An array to store the test numbers.
    int thread_count;                                       // A count of how many threads have been completed.
    pthread_mutex_t test_mutex;                             // Mutex for the completed thread count.
};


// Create a structure to pass arguments to each server thread.
struct ThreadArgsServer
{
    struct Memory* shared_data;
    int number;
    int current_index;
};

// Create a structure to pass arguments to each client thread.
struct ThreadArgsClient
{
    struct Memory* shared_data;
    sem_t *sem_client;
    sem_t *sem_server; 
};

// Create a structure to pass arguments to each test thread.
struct ThreadArgsTest
{
    struct Memory* shared_data;
    sem_t *sem_client;
    sem_t *sem_server; 
    int case_number;
    int thread_number;
};


unsigned long binary_to_decimal(int* binary_array);
void clear_buffer();
void* client_connected(void* args);
void create_server_threads(struct Memory* shared_data);
void decimal_to_binary(int* binary_array, unsigned long n);
int generate_random_number();
void generate_test_numbers(int* arr, int n);
void pop_request_queue(struct Memory* shared_data);
void print_array(int* binary_array, int n);
void print_array_long(unsigned long* num_array, int n);
void* print_client_processes(void* args);
void push_request_queue(struct Memory* shared_data, int i);
void rotate_binary(int* binary_array);
void server_connected(struct Memory* shared_data, sem_t *sem_client, sem_t *sem_server);
void* server_thread(void* args);
void* test_mode(void* args);
void terminate_process(struct Memory* shared_data, int i);
unsigned long* trial_division_factorise(unsigned long number, int *size);
void trigger_process(struct Memory* shared_data, int index, int number);
bool valid_input(char* str);