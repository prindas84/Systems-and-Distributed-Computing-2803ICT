# Compile both server files together.
all: server client

# Compile both server files individually with required functions.
server: server.o server_functions.o functions.o
	gcc -g -o server server.o server_functions.o functions.o -lrt

client: client.o client_functions.o functions.o
	gcc -g -o client client.o client_functions.o functions.o -lrt

# Compile object files for each file that is required above..
server.o: server.c include.h
	gcc -g -c server.c

server_functions.o: server_functions.c include.h
	gcc -g -c server_functions.c

client.o: client.c include.h
	gcc -g -c client.c

client_functions.o: client_functions.c include.h
	gcc -g -c client_functions.c

functions.o: functions.c include.h
	gcc -g -c functions.c