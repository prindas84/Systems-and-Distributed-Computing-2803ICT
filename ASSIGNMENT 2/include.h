#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 10
#define BUFFER 128
#define HOST_SIZE 256
#define MAX_ERRORS 5
#define MINIMUM_PLAYERS 2
#define TIMEOUT 30
#define WINNING_TOTAL 30

// Message type enumeration for readability.
enum message_types { WELCOME, START, TEXT, GO, END, MOVE, QUIT };

// Function Prototypes/
int argument_count(char* user_input);
void client_play(int client_socket);
void client_send(int message_types, char* message, int client_socket);
bool is_number(char *text_message);
void print_hostname();
void server_play(int* client_socket, int player_count);
void server_send(int message_types, char* message, int client_socket, int i);