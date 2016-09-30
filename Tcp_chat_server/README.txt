# Server-Client in C/C++
# Computer Network Project - Example

This simple Server-Client programs are provided so that you get glimpse of how socket APIs can be used.
The server awaits for a client and send a message if one connects. Then it closes both sockets of client and server itself, and terminates.
The client connects to the server, prints the message passed by the server, and terminates.

Be sure to understand each of socket APIs used in this source files, and modify them accordingly.
For example, Server and Client are now communicating using TCP, but you are asked to use UDP instead.

Also, note that the connect() function returns 0 on success and -1, otherwise.
We may assume that the server is already running beforehand a client tries to connect in our assignment.
But still, it will be a good practice to handle the case when it fails to connect (that is, the return value is -1).

	e.g.	if (connect(sock_fd, (struct sockaddr*) &server_addr,
                	sizeof(server_addr)) == -1) {
			perror("connect");
			exit(1);
		}

Likewise, try to understand what each function does and make your code nice and clean.

To get started, remotely log into the server and implement your own server and client programs.
You may need to download an SSH client such as Putty.

You may test your program with following steps:
1. Open up two sessions connected to the server
2. Execute ¡°server¡± on one side, and ¡°client¡± on the other
3. recommanded way is running client in your own machine, and server in your homework server. 

If you have any question, see reference in homework specification PDF file, or post via ETL.
Good luck!



