#include "include.h"

// Declare global pointers to assist in the timeout alarm handler.
int* cs_ptr = NULL;

void alarm_handler(int signum)
{
    char server_message[BUFFER];

    // Send intructions to the server to quit the current client.
    client_send(QUIT, "QUIT\n", *cs_ptr);
    printf("\n\nClient Timeout - Disconnecting you now...\n");

    // Terminate the client program.
    close(*cs_ptr);
    exit(0);
}

void client_play(int client_socket)
{
    char client_buffer[BUFFER];
    bool connected = false;
    int result;
    char server_message[BUFFER];
    cs_ptr = &client_socket;

    // Setup the alarm handler.
    signal(SIGALRM, alarm_handler);
    
    // Wait for all players to be connected to the server to begin the game.
    memset(server_message, '\0', BUFFER);
    while (!connected)
    {
        // Check if the server has started a new game.
        recv(client_socket, server_message, sizeof(server_message) - 1, 0);
        if (strcmp(server_message, "GAME STARTED") == 0)
        {
            connected = true;
            printf("\nALL PLAYERS HAVE CONNECTED - THE GAME WILL NOW BEGIN SHORTLY\nPLEASE WAIT FOR YOUR TURN...\n");
        }
    }

    // While the client is connected, continuously recieved and send messages to and from the server.
    while (connected)
    {
        // Receive a message from the server.
        result = 0;
        memset(server_message, '\0', BUFFER);
        result = recv(client_socket, server_message, sizeof(server_message) - 1, 0);

        // If a message has been recieved...
        if (result > 0)
        {
            // If all other players have disconnected, terminate the connection.
            if (strcmp(server_message, "TEXT You Won!") == 0)
            {
                printf("\nYou Won!\n");
                printf("\nPlease Wait - Disconnecting you now...\n");
                return;
            }

            // If the message received is "END", terminate the connection.
            else if (strcmp(server_message, "END") == 0)
            {
                printf("\nPlease Wait - Disconnecting you now...\n");
                connected = false;
                return;
            }

            // If the message is a TEXT message, split it and print accordingly.
            else if (strncmp(server_message, "TEXT ", 5) == 0) 
            {
                // Split the line at the " " and use remaining as the move number.
                char* text_message = strchr(server_message, ' ');

                // If there is no error, remove the first character of the string " ".
                if (text_message != NULL)
                {
                    text_message += 1;
                }
                printf("\n%s", text_message);
            }

            // If the message is a GO instruction, prompt the user to make a move.
            else if (strcmp(server_message, "GO") == 0) 
            {
                // Start the timer to measure TIMEOUT.
                alarm(TIMEOUT);
                
                // Get the move from the player.
                memset(client_buffer, '\0', BUFFER);
                fgets(client_buffer, sizeof(client_buffer), stdin);

                // Stop the timer to measure TIMEOUT if a message has been received.
                alarm(0);

                // If the user input was "QUIT" then terminate the connection.
                if (strcmp(client_buffer, "QUIT\n") == 0)
                {
                    // Send intructions to the server and exit.
                    client_send(QUIT, client_buffer, client_socket);
                    printf("\nPlease Wait - Disconnecting you now...\n");
                    connected = false;
                    return;
                }

                // If the user makes a move...
                else
                {
                    // Send instructions to the server.
                    client_send(MOVE, client_buffer, client_socket);
                }
            }
            // If there is a protocol error...
            else
            {
                // Send intructions to the server and exit.
                client_send(QUIT, client_buffer, client_socket);
                printf("\nERROR: Protocol Infringement (Faulty Client)\n");
                printf("\nPlease Wait - Disconnecting you now...\n");
                connected = false;
                return;
            }
        }
    }
    return;
}