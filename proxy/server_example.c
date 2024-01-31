// This program implements a server that accepts 
// a connection from a client and copies the received 
// bytes to a file
//
// The input arguments are as follows:
// argv[1]: Sever's port number

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

int main (int argc, char *argv[]) {

	char    message[10] = "received!";  // message to be sent to the client when the destination file name is received
	int     net_bytes_read;             // number of bytes received over socket
	int     socket_fd = 0;              // socket descriptor
	int     connection_fd = 0;          // new connection socket descriptor
	struct  sockaddr_in serv_addr;      // address format structure
	char    net_buff[1024];             // buffer to hold characters read from socket
	int		backlog = 40;				// max queue size for listen()
	FILE    *dest_file;                 // pointer to the file that will include the received bytes over socket

	if (argc < 2) {	// Note: the name of the program is counted as an argument
		printf("Port number not specified!\n");
		return 1;
	}

	// Create a socket
	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error, creating a scoket\n");
		return 1;
	}
	
	// Specify address for the socket and other things
	int port;
	port = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;			// Internet Protocol version 4 (IPv4) protocol family
	serv_addr.sin_port = htons(port);		// I think this is the port where we will listen, so like 8046
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// This is like a wildcard, listen on all interfaces

	// Call the bind() function
	int myBind;
	if((myBind = bind(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))) < 0) {
		printf("Error, when binding\n");
		return 1;
	}

	// Call the listen() function
	int myListen;
	if((myListen = listen(socket_fd, backlog)) < 0) {
		printf("Error, when listening\n");
		return 1;
	}

	// Call the accept() function
	if((connection_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL)) < 0) {
		printf("Error, when connecting\n");
		return 1;
	}

	// == Application begins here ==
	
	// Receive filename from client
	bzero(net_buff, sizeof(net_buff));
	net_bytes_read = read(connection_fd, net_buff, sizeof(net_buff));
	if (net_bytes_read < 0) printf("ERROR when reading from socket\n");

 	printf("Filename received from client: %s\n", net_buff);
	printf("Read %d bytes from client\n", net_bytes_read);

	char recvFilename[net_bytes_read];
	for(int i = 0; i < net_bytes_read; i++) {
		recvFilename[i] = net_buff[i];
	}

	// Reply to client
	int net_bytes_wrote;

	net_bytes_wrote = write(connection_fd, net_buff, net_bytes_read);
    if(net_bytes_wrote < 0) printf("ERROR writing to socket\n");

	// Open output file and receive and write the data from client in chunks
	int chunk = 10;
	dest_file = fopen(recvFilename, "w");
	bzero(net_buff, sizeof(net_buff));

	while((net_bytes_read = read(connection_fd, net_buff, chunk)) > 0) {

		printf("Receiving %d bytes from client\n", net_bytes_read);

		for(int i = 0; i < net_bytes_read; i++) {
			printf("Received characters: %c\n", net_buff[i]);
		}
		printf("\n");

		net_bytes_wrote = fwrite(net_buff, sizeof(char), net_bytes_read, dest_file);
		if (net_bytes_wrote <= 0) printf("ERROR writing to output.txt\n");

		printf("Message wrote to output.txt: %s\n", net_buff);
		bzero(net_buff, sizeof(net_buff));
	}

	close(socket_fd);

	return 0;
}

