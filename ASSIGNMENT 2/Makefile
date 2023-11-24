# Compile both server files together.
all: game_server game_client

# Compile both server files individually with required functions.
game_server: game_server.o server_play.o functions.o
	gcc -g -o game_server game_server.o server_play.o functions.o

game_client: game_client.o client_play.o functions.o
	gcc -g -o game_client game_client.o client_play.o functions.o

# Compile O files for each file that is required above..
game_server.o: game_server.c
	gcc -g -c game_server.c

game_client.o: game_client.c
	gcc -g -c game_client.c

server_play.o: server_play.c
	gcc -g -c server_play.c

client_play.o: client_play.c
	gcc -g -c client_play.c

functions.o: functions.c
	gcc -g -c functions.c


	