#include "include.h"

int main(int argc, char *argv[])
{
    // Check for correct argument count.
    if (argc != 4) 
    {
        printf("USAGE: ./game_server <Port Number: 4444> <Game Type: numbers> <Game Arguments: 3>\n");
        exit(1);
    }

    // Convert the arguments to variables.
    int port = atoi(argv[1]);
    char* game_type = argv[2];
    int player_count = atoi(argv[3]);

    // Declare the variables required to complete a server connection.
    struct sockaddr_in server, client;
    int client_size = sizeof(client);
    int* client_socket = (int*)calloc(player_count, sizeof(int));       
    int result;    
    int server_socket;    

    // Print the server hostname to allow for connections by a client.
    print_hostname();

    /*  Initisalise the sockaddr_in structure accoding to documentation.
    1. Set the protocol family.
    2. Set the port to listen to (in correct endianess).
    3. Allow all IP addresses to bind to this connection (INADDR_ANY). */
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    // Create the TCP socket.
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("ERROR: Unable to create a socket.\n");
        free(client_socket);
        close(server_socket);
        exit(1);
    }
    printf("Socket initialised...\n");

    /*  Bind the address to the socket.
    1. The socket that we wish to bind is server_socket which was created above.
    2. We want to cast a pointer to the "server" variable as a "struct sockaddr" pointer to allow the bind.
    3. The "struct sockaddr" casting will need to be the size of the "server" variable */
    result = bind(server_socket, (struct sockaddr *) &server, sizeof(server));
    if(result < 0)
    {
        printf("ERROR: Unable to bind the socket.\n");
        free(client_socket);
        close(server_socket);
        exit(1);
    }
    printf("Bind completed...\n");

    /*  Listen for incoming connections.
    1. Listen to the server_socket socket which was created above.
    2. Set the maximum number of connections that can queue while other connections are being processed */
    result = listen(server_socket, BACKLOG);
    if(result != 0)
    {
        printf("ERROR: Unable to listen for connections.\n");
        free(client_socket);
        close(server_socket);
        exit(1);
    }
    printf("Waiting for incoming connections...\n");

    // Loop until all required players have connected.    
    for (int i = 0; i < player_count; i++)
    {
        /*  Accept a connection from a client. The sizeof(client) must be converted to an int first, as per documentation.
        1. The socket that we wish to accept connections too is server_socket.
        2. We want to cast a pointer to the "client" variable as a "struct sockaddr" pointer to allow the conenction.
        3. The "struct sockaddr" casting will need to be the size of the "client" variable, which has already been converted to an int */
        client_socket[i] = accept(server_socket, (struct sockaddr *) &client, &client_size);
        if (client_socket[i] < 0)
        {
            printf("ERROR: Unable to accept the client connection.\n");
            for (int j = 0; j < i; j++)
            {
                close(client_socket[j]);
            }
            free(client_socket);
            close(server_socket);
            exit(1);
        } 
        else
        {
            // Send welcome message to the player.
            printf("PLAYER %d: Connection established...\n", i + 1);
            server_send(WELCOME, NULL, client_socket[i], i);
        }
    }
    printf("THE GAME HAS COMMENCED...\n");
    sleep(2);
    
    // Commence the game with the connected players.
    server_play(client_socket, player_count);

    // Free all allocated memory and close the socket connections once the game has finished.
    for (int k = 0; k < player_count; k++) 
    {
        close(client_socket[k]);
    }
    free(client_socket);
    close(server_socket);

    return 0;
}
