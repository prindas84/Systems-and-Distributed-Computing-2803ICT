#include "include.h"

int argument_count(char* user_input)
{
    // Initialise the word count to 1, then loop through the original input incrementing count on ' '.
    int count = 1;

    // Check that the last character is not a space. Remove it if it is.
    if (user_input[strlen(user_input) - 1] == ' ')
    {
        user_input[strlen(user_input) - 1] = '\0';
    }

    // Iterate over the string and count the spaces to find the number of words.
    for (int i = 0; user_input[i] != '\0'; i++)
    {
        if (user_input[i] == ' ')
        {
            count++;
        }
    }
    return count;
}


void client_send(int message_types, char* message, int client_socket)
{
    // Declare the server message and arrange it accordingly.
    char client_buffer[BUFFER];
    memset(client_buffer, '\0', BUFFER);

    // Send the message from the client to the server accodingly.
    switch (message_types) 
    {
        case MOVE:
            snprintf(client_buffer, sizeof(client_buffer), "MOVE %s", message);    
            send(client_socket, client_buffer, sizeof(client_buffer) - 1, 0);
            break;
        case QUIT:
            send(client_socket, message, sizeof(message) - 1, 0);
            break;
        default:
            printf("ERROR: Unable to process message...");
            break;
    }
    return;
}


bool is_number(char *text_message)
{
    // Iterate through each character in the string to check if it is a digit. Return false if character found.
    for (int i = 0; text_message[i] != '\0'; i++)
    {
        if (!isdigit(text_message[i]))
        {
            return false;
        }
    }
    return true;
}


void print_hostname()
{
    // Print the server hostname to the server shell to allow for connections by a client.
    char hostname[HOST_SIZE];
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        printf("Hostname: %s\n", hostname);
    } 
    else 
    {
        printf("ERROR: Unable to find Hostname.\n");
    }
    return;
}


void server_send(int message_types, char* message, int client_socket, int i)
{
    // Declare the server message and arrange it accordingly.
    char server_message[BUFFER];
    memset(server_message, '\0', BUFFER);

    // Send the message from the server to the client accodingly.
    switch (message_types) 
    {
        case WELCOME:
            snprintf(server_message, sizeof(server_message), "Player %d - Welcome to the game! Please wait for all players to connect...", i + 1);
            send(client_socket, server_message, sizeof(server_message) - 1, 0);
            break;
        case START:
            snprintf(server_message, sizeof(server_message), "GAME STARTED");
            send(client_socket, server_message, sizeof(server_message) - 1, 0);
            break;
        case TEXT:
            snprintf(server_message, sizeof(server_message), "TEXT %s", message);
            send(client_socket, server_message, sizeof(server_message) - 1, 0);
            break;
        case GO:
            snprintf(server_message, sizeof(server_message), "GO");
            send(client_socket, server_message, sizeof(server_message) - 1, 0);
            break;
        case END:
            snprintf(server_message, sizeof(server_message), "END");
            send(client_socket, server_message, sizeof(server_message) - 1, 0);
            break;
        default:
            printf("ERROR: Unable to process message...");
            break;
    }
    return;
}