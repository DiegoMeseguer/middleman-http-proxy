// This program implements a client that connects to a server and 
// transfers the bytes read from a file over the established connection
//
// The input arguments are as follows:
// argv[1]: Sever's IP address
// argv[2]: Server's port number
// argv[3]: Source file
// argv[4]: Destination file at the server side which includes a copy of the source file

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	int		sockfd = 0;				// socket descriptor
	char	net_buff[1024];			// to hold the bytes read from socket
	char	file_buff[80];			// to hold the bytes read from source file
	struct	sockaddr_in serv_addr;	// server address structure
	int		net_bytes_read;			// numer of bytes received over socket
	int		net_bytes_wrote;		// number of bytes wrote over socket
	FILE	*source_file;			// pointer to the source file

	// Make sure intial values are zero
	memset(net_buff, '0', sizeof (net_buff));
	memset(file_buff, '0', sizeof (file_buff));
	memset(&serv_addr, '0', sizeof (serv_addr));

	if(argc < 5) {
		printf ("Usage: ./%s <server ip address> <server port number> <source file>  <destination file>\n", argv[0]);
		return 1;
	}
	
	// Create a socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error when creating a socket\n");
		return 1;
	}

    // Specify address for the socket and other things
	int port;
	port = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;					// Internet Protocol version 4 (IPv4) protocol family
    serv_addr.sin_port = htons(port);				// I think this is the port where the SERVER will be listening
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr);	// This is the IP of the server, I think we are
																// writing it to the struct directly

    // Call the connect() function
    int status;
    if((status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
		printf("Error when connecting\n");
		return 1;
	}

	// == Application begins here ==
	
	// Send filename to server
	//printf("This is string %lu\n", strlen(argv[4]) + 1);
	net_bytes_wrote = write(sockfd, argv[4], strlen(argv[4]) + 1);
	if(net_bytes_wrote < 0) printf("ERROR writing to socket");
	
 	// Print response from server
	bzero(net_buff, sizeof(net_buff));
	net_bytes_read = read(sockfd, net_buff, sizeof(net_buff));
    if(net_bytes_read < 0) printf("ERROR reading from socket");

    printf("Here is the reply from the server: %s\n", net_buff);

	// Open the input file into memory
	source_file = fopen(argv[3], "r");

    // Transfer input file of client to output file of server in chunks of 10 bytes
	bzero(net_buff, sizeof(net_buff));
	int chunk = 10;
	while((net_bytes_read = fread(net_buff, sizeof(char), chunk, source_file)) > 0) {

		printf("Sending %d bytes to server\n", net_bytes_read);

		for(int i = 0; i < net_bytes_read; i++) {
			printf("Read characters: %c\n", net_buff[i]);
		}
		printf("\n");

		net_bytes_wrote = write(sockfd, net_buff, net_bytes_read);
		if (net_bytes_wrote < 0) printf("ERROR writing to socket\n");

		printf("Message sent to server: %s\n", net_buff);
		bzero(net_buff, sizeof(net_buff));
	}

	close(sockfd);	
	
	return 0;
}

