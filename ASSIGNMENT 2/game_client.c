#include "include.h"

int main(int argc, char *argv[])
{
    // Check for correct argument count.
    if (argc != 4) 
    {
        printf("USAGE: ./game_client <Game Type: numbers> <Server Name: mypc> <Port Number: 4444>\n");
        exit(1);
    }

    // Convert the arguments to variables.
    char* game_type = argv[1];
    char* hostname = argv[2];
    int port = atoi(argv[3]);

    // Resolve IP Address from server name.
    struct addrinfo address_struct, *server_info;

    // Declare the variables required to complete a server connection.
    struct sockaddr_in server;
    int client_socket;
    int result;
    char server_message[BUFFER];

    // Set the memory at the address structure to '0' - default.
    // Initialise the ai_family to IPv4 (AF_INET) and the socktype to TCP (SOCK_STREAM) to set the defaults we want to use later.
    memset(&address_struct, 0, sizeof(address_struct));
    address_struct.ai_family = AF_INET;
    address_struct.ai_socktype = SOCK_STREAM;

    /*  Extraxt the server information.
    1. Use the "hostname" to find the computer on the network. 
    2. Do not declare a port to search (NULL)
    3. Use the defaults set in the address_struct
    4. Copy the information (using defaults from address_struct) to the server_info struct */
    result = getaddrinfo(hostname, NULL, &address_struct, &server_info);
    if (result != 0)
    {
        printf("ERROR: Unable to locate the server.\n");
        exit(1);
    }

    /*  Initisalise the sockaddr_in structure accoding to documentation.
    1. Set the protocol family.
    2. Set the port to listen to (in correct endianess).
    3. Resolve the IP for the hostname for the computer we want to connect too. */
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = ((struct sockaddr_in *)(server_info->ai_addr))->sin_addr.s_addr;

    // Create the TCP socket.
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        printf("ERROR: Unable to create a socket.\n");
        freeaddrinfo(server_info);
        close(client_socket);
        exit(1);
    }
    printf("Socket initialised...\n");

    /*  Connect to a remote server.
    1. The socket that we wish to connect with is client_socket which was created above.
    2. We want to cast a pointer to the "server" variable as a "struct sockaddr" pointer to allow the connection.
    3. The "struct sockaddr" casting will need to be the size of the "server" variable */
    result = connect(client_socket, (struct sockaddr *) &server, sizeof(server));
    if (result == -1)
    {
        printf("ERROR: Unable to connect to the server.\n");
        freeaddrinfo(server_info);
        close(client_socket);
        exit(1);
    }
    printf("Connection established...\n");

    // Receive the welcome message from the server.
    memset(server_message, '\0', BUFFER);
    result = recv(client_socket, server_message, sizeof(server_message) - 1, 0);
    if (result == -1)
    {
        printf("ERROR: Unable to receive welcome message from the server.\n");
        freeaddrinfo(server_info);
        close(client_socket);
        exit(1);
    }
    printf("%s\n", server_message);

    // Enter the game to play.
    client_play(client_socket);

    // Close sockets and free memory before terminating.
    freeaddrinfo(server_info);
    close(client_socket);
    
    return 0;
}