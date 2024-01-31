// This program implements a web server
//
// The input arguments are as follows:
// argv[1]: Sever's port number

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#define BUFF_SIZE      20000
#define HTML_FILE      "index.html"
#define NOT_FOUND      "404.html"

int main (int argc, char *argv[]) {
    
    int        socket_fd = 0;               // socket descriptor
    int        connection_fd = 0;           // new connection descriptor
    int        net_bytes_read;              // numer of bytes received over socket
    int        net_bytes_wrote;             // number of bytes send over socket
    int        backlog = 40;                // max queue size for listen()
    struct     sockaddr_in serv_addr;       // Address format structure
    char       net_buff[BUFF_SIZE];         // buffer to hold request messages from client
    char       body[BUFF_SIZE];             // buffer to hold characters read from socket
    char       head[BUFF_SIZE];             // buffer to hold characters read from socket
    FILE       *source_file;                // pointer to the source file
    
    if(argc < 2) {
        printf ("Port number not specified!\n");
        return 1;
    }

    // Create a socket
    if((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error: Could not create socket! \n");
        return 1;
    }

    // Enable socket options. To prevent "Address in use" error
    // The SO_REUSEADDR socket option, which explicitly allows a process to bind to a port which remains in TIME_WAIT
	const int enable = 1;
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
 	    printf("setsockopt(SO_REUSEADDR) failed\n");
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
	if(listen(socket_fd, backlog) < 0) {
		printf("Error, when listening\n");
		return 1;
	}
    else {
        printf("Listening to incoming connections... \n");
    }


	// Call the accept() function
	if((connection_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL)) < 0) {
		printf("Error, when connecting\n");
		return 1;
	}
    else {
        printf("Incoming connection: Accepted! \n");
    }


    // == Application begins here ==

    // Receive HTTP request message from client
    bzero(net_buff, sizeof(net_buff));
    net_bytes_read = read(connection_fd, net_buff, sizeof(net_buff));
    if (net_bytes_read < 0) printf("ERROR when reading from socket\n");

    printf("Received %d bytes. This is the HTTP request:\n", net_bytes_read);
    printf("%s\n", net_buff);

    // Parse the HTTP request message
    printf("Parsing HTTP request...\n");
    printf("...\n");

    char* serverFile = strtok(net_buff, " ");
    serverFile = strtok(NULL, " ");
    serverFile = serverFile + 1;
    
    // Check if the HTTP request is valid and act accordingly
    int fileFound = strcmp(serverFile, HTML_FILE);

    if(fileFound == 0) {
        // Open the index.html file of the server
        source_file = fopen("index.html", "r");
        if(source_file == NULL) printf("Error when reading index.html\n");

        // Load the index.html file to memory (body of response message)
        // Note: We are assuming that max size of index.html is 20000 bytes
        bzero(body, sizeof(body));
    
	    net_bytes_read = fread(body, sizeof(char), BUFF_SIZE, source_file);
        if (net_bytes_read <= 0) printf("ERROR reading from index.html\n");
        printf("Read index.html file of %d bytes\n", net_bytes_read);

        // Create the header portion of response message
        // Note: Omit Content-Length for now
        // Old: char httpHeader[] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 2000000\r\n";
	    // Old: int headerLength = strlen(httpHeader);
        // Old Old: int fileSize = calc_filesize(HTML_FILE);
        char httpHeader[4096];
        int headerLength;
        bzero(httpHeader, sizeof(httpHeader));

        snprintf(httpHeader, sizeof(httpHeader), "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\r\n", net_bytes_read);
        headerLength = strlen(httpHeader);

        // Combine header with body to form HTTP response message
        bzero(head, sizeof(head));

        for(int i = 0; i < headerLength; i++) {
           head[i] = httpHeader[i];
        }

        for(int i = 0; i < net_bytes_read; i++) {
            head[headerLength + i] = body[i];
	    }

        printf("The HTTP response message to be send is:\n");
        printf("%s\n", head);
    
        // Send web page to client (browser)
        net_bytes_wrote = write(connection_fd, head, strlen(head) + 1);
        if(net_bytes_wrote < 0) printf("ERROR writing to socket\n");

        printf("Size of HTTP response message: %lu\n", strlen(head)+ 1);
        printf("Size of HTTP response header: %lu\n", strlen(httpHeader));
        printf("Reply sent to the client!\n");
    }
    else {
        // Open the 404.html file of the server
        source_file = fopen(NOT_FOUND, "r");
        if(source_file == NULL) printf("Error when reading 404.html\n");

        // Load the 404.html file to memory (body of response message)
        // Note: We are assuming that max size of 404.html is 20000 bytes
        bzero(body, sizeof(body));
    
	    net_bytes_read = fread(body, sizeof(char), BUFF_SIZE, source_file);
        if (net_bytes_read <= 0) printf("ERROR reading from index.html\n");
        printf("Read 404.html file of %d bytes\n", net_bytes_read);

        // Prepare the 404 response header
        char HTTP404[] = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 239\r\n";
        int headerLength;
        headerLength = strlen(HTTP404);

        // Prepare the 404 response message by combining header and body
        bzero(head, sizeof(head));

        for(int i = 0; i < headerLength; i++) {
            head[i] = HTTP404[i];
        }

        for(int i = 0; i < net_bytes_read; i++) {
            head[headerLength + i] = body[i];
        }

        printf("The HTTP response message to be send is:\n");
        printf("%s\n", head);

        // Send response to client (browser)
        net_bytes_wrote = write(connection_fd, head, strlen(head) + 1);
        if(net_bytes_wrote < 0) printf("ERROR writing to socket\n");

        printf("404 Not Found sent to the client!\n");
    }

    //shutdown(connection_fd, SHUT_RDWR);
    close(connection_fd);
	close(socket_fd);
	
}

