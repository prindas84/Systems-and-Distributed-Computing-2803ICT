#include "include.h"

void server_play(int* client_socket, int player_count)
{

    // Send a message to each player indicating that the game has started.
    for (int k = 0; k < player_count; k++)
    {
        server_send(START, NULL, client_socket[k], k);
    }

    // Declare the variables required to commence the game.
    char client_buffer[BUFFER];
    int connected_players = player_count;
    int error_count = 0;
    int game_sum = 0;
    int move_value = 0;
    bool playing = true;
    char server_message[BUFFER];

    // Create a boolean array to monitor player connections. Set all values to true
    bool* connections = (bool *)malloc(player_count * sizeof(bool));
    if (connections == NULL)
    {
        printf("ERROR: Unable to allocate memory to the connections array.");
        return;
    }
    for (int i = 0; i < player_count; i++)
    {
        connections[i] = true;
    }

    // Begin the game.
    while (playing)
    {
        // Iterate through all players in the order they connected.
        for (int i = 0; i < player_count; i++)
        {
            // If the player has disconnected from the game, skip the player.
            if (connections[i] == false)
            {
                continue;
            }

            // If there are less than MINIMUM PLAYERS left, notify the final player and end the game.
            else if (connected_players < MINIMUM_PLAYERS)
            {
                server_send(TEXT, "You Won!", client_socket[i], i);
                printf("\nGAME OVER: The server is disconnecting...\n");
                playing = false;
            }

            // If there are at least MINIMUM PLAYERS players and the current player is still connected...
            else
            {
                // While the user has made less than MAX_ERRORS on this iteration...
                error_count = 0;
                while (error_count <= MAX_ERRORS)
                {
                    // If the player has made MAX_ERRORS, terminate the session.
                    if (error_count == MAX_ERRORS)
                    {
                        connections[i] = false;
                        connected_players--;

                        // Send an intruction to the player to make their move.
                        server_send(END, NULL, client_socket[i], i);
                        break;
                    }
                    // If the player has not reached their maximum number of error attempts...
                    else
                    {
                        // Send a message to the player indicating that it is their turn.
                        snprintf(server_message, sizeof(server_message), "------TYPE 'QUIT' TO LEAVE THE GAME------\nIt's your turn...\nThe Game Total is %d. Enter a number: ", game_sum);
                        server_send(TEXT, server_message, client_socket[i], i);

                        // Send an intruction to the player to make their move.
                        server_send(GO, NULL, client_socket[i], i);

                        // Wait for the player to reply...
                        memset(client_buffer, '\0', BUFFER);
                        recv(client_socket[i], client_buffer, BUFFER, 0);
                        printf("PLAYER %d: %s", i + 1, client_buffer);

                        // If the user wants to quit the game, terminate the session
                        if (strcmp(client_buffer, "QUIT\n") == 0)
                        {
                            connections[i] = false;
                            connected_players--;
                            break;
                        }

                        // If the user has replied with a MOVE request...
                        else if (strncmp(client_buffer, "MOVE ", 5) == 0) 
                        {
                            // If the instruction in invalid (incorrect number of arguments)...
                            if (argument_count(client_buffer) != 2)
                            {
                                // Send a message to player indicating a handling error.
                                error_count++;
                                server_send(TEXT, "ERROR: Usage - <Number: (1 to 9)>\n", client_socket[i], i);
                                continue;
                            }

                            // If the MOVE request has the correct number of arguments.
                            else
                            {
                                // Trim the \n from the end of the string.
                                client_buffer[strlen(client_buffer) - 1] = '\0';

                                // Split the line at the " " and use remaining as the MOVE number.
                                char* text_message = strchr(client_buffer, ' ');

                                // If there is no error, remove the first character of the string " ".
                                if (text_message != NULL)
                                {
                                    text_message += 1;
                                }

                                // Check that the argument is a number between 1 - 9...
                                if (is_number(text_message))
                                {
                                    move_value = atoi(text_message);
                                    if (move_value >= 1 && move_value <= 9)
                                    {
                                        // If the move is valid, increment the game sum and move to the next player.
                                        game_sum += move_value;

                                        // If the player has won the game, end accordingly.
                                        if (game_sum >= WINNING_TOTAL)
                                        {
                                            // Send a message to the player indicating they won.
                                            server_send(TEXT, "You Won!", client_socket[i], i);
                                            connections[i] = false;

                                            // Iterate through all remaining connected player to let them know they have lost.
                                            for (int k = 0; k < player_count; k++)
                                            {
                                                if (connections[k])
                                                {
                                                    server_send(TEXT, "Sorry, You Lost...\n", client_socket[k], k);
                                                    server_send(END, NULL, client_socket[k], k);
                                                }
                                            }
                                            printf("\nGAME OVER: The server is disconnecting...\n");
                                            // Set playing to false to stop the loop.
                                            playing = false;
                                        }

                                        // If the game needs to continue...
                                        server_send(TEXT, "Please Wait...\n", client_socket[i], i);
                                        break;
                                    }

                                    // If the instruction in invalid (outside of the acceptabe range)...
                                    else
                                    {
                                        // Send a message to player indicating a handling error.
                                        error_count++;
                                        server_send(TEXT, "ERROR: Usage - <Number: (1 to 9)>\n", client_socket[i], i);
                                        continue;
                                    }
                                }

                                // If the instruction in invalid (not a number)...
                                else
                                {
                                    // Send a message to player indicating a handling error.
                                    error_count++;
                                    server_send(TEXT, "ERROR: Usage - <Number: (1 to 9)>\n", client_socket[i], i);
                                    continue;
                                } 
                            }
                        }
                        else
                        {
                            // If there is some kind of protocol infringement by not matching the rules above...
                            server_send(TEXT, "ERROR: Protocol Infringement (Faulty Client)\n", client_socket[i], i);
                            server_send(END, NULL, client_socket[i], i);
                        }
                    }
                }
            }
            // If the game has ended, break the loop.
            if (!playing)
            {
                break;
            }
        }
    }
    // Free the allocated memory.
    free(connections);
    return;
}