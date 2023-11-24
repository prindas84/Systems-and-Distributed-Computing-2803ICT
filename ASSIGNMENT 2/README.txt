GAME SERVER

USAGE - 	./game_server <Port Number: 4444> <Game Type: numbers> <Game Arguments: 3>
EXAMPLE - 	./game_server 8080 numbers 3
DIRECTIONS - 	Once the server has been initialised, provide all game clients with the PORT NUMBER, as well as the
		SERVER NAME, which are required to connect to the network.
		
		The server name can be found in the game server shell after a connection has been established.

		--------------------------------------------------------------------------
		Hostname: LAPTOP-L5VKF6H5
		Socket initialised...
		Bind completed...
		Waiting for incoming connections...
		--------------------------------------------------------------------------

		When the required number of players have connected to the server, the game will begin.

		--------------------------------------------------------------------------
		PLAYER 1: Connection established...
		PLAYER 2: Connection established...
		PLAYER 3: Connection established...
		THE GAME HAS COMMENCED...
		--------------------------------------------------------------------------

		When the game has finished, the server will disconnect and shut down.

		--------------------------------------------------------------------------
		GAME OVER: The server is disconnecting...
		--------------------------------------------------------------------------



GAME CLIENT

USAGE - 	./game_client <Game Type: numbers> <Server Name: LAPTOP-L5VKF6H5> <Port Number: 4444>
EXAMPLE - 	./game_client numbers LAPTOP-L5VKF6H5 8080
DIRECTIONS -	Once the game client has been initialised it will connect to the game server and wait for the required number
		of players to connect to the game. You will be notified that you connection is complete.

		--------------------------------------------------------------------------
		Player 1 - Welcome to the game! Please wait for all players to connect...
		--------------------------------------------------------------------------

		When all players have connected, you will be notified that the game has begun. Please wait for your turn.

		--------------------------------------------------------------------------
		ALL PLAYERS HAVE CONNECTED - THE GAME WILL NOW BEGIN SHORTLY
		PLEASE WAIT FOR YOUR TURN...
		--------------------------------------------------------------------------

		When it is your turn to play, please enter a number between 1 - 9. The aim of the game is to be the first
		player to reach the goal sum, which is normally 30. Each player is allowed to make 5 invalid inputs before
		they are disconnected from the game. These numbers can be defined in the includes.h file.

		If you would like to exit the game, when it is your turn, you can type "QUIT".

		--------------------------------------------------------------------------
		------TYPE 'QUIT' TO LEAVE THE GAME------
		It's your turn...
		The Game Total is 0. Enter a number: 
		--------------------------------------------------------------------------

		After submitting your move, please wait until it is your turn again. The game will continue through this
		process until someone has won, or too many players have disconnected.

		Eventually, all players are notified if they have won the game, or lost, and then they are disconnected.

		--------------------------------------------------------------------------
		You Won!

		Please Wait - Disconnecting you now...
		--------------------------------------------------------------------------
