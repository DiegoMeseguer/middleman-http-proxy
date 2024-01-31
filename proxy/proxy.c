// Tested on Firefox 113.0.1 and above
// == Some websites that work ==
// http://neverssl.com/
// httpforever.com/

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
#include <netdb.h>

const unsigned int BUFF_SIZE = 2000000;

int main (int argc, char *argv[]) {

    int serverSocketFD = 0;
    int serverConnFD = 0;
    int clientSocketFD = 0;
    int backlog = 40;
    char webPort[] = "80";
    int netBytesRead, netBytesWrote;
    char net_buff[BUFF_SIZE];
    struct sockaddr_in serv_addr;
    struct addrinfo hints, *res;

	if (argc != 2) {	// Note: the name of the program is counted as an argument
		printf("Port number not specified!\n");
		return 1;
	}

	// Create a socket
	if((serverSocketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error, creating a scoket\n");
		return 1;
	}

    // Enable socket options. To prevent "Address in use" error
    // The SO_REUSEADDR socket option, which explicitly allows a process to bind to a port which remains in TIME_WAIT
	const int enable = 1;
	if(setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
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
	if((myBind = bind(serverSocketFD, (struct sockaddr*) &serv_addr, sizeof(serv_addr))) < 0) {
		printf("Error, when binding\n");
		return 1;
	}

	// Call the listen() function
	if(listen(serverSocketFD, backlog) < 0) {
		printf("Error, when listening\n");
		return 1;
	}
    else {
        printf("Listening to incoming connections... \n");
    }

	// Call the accept() function
	if((serverConnFD = accept(serverSocketFD, (struct sockaddr *)NULL, NULL)) < 0) {
		printf("Error, when connecting\n");
		return 1;
	}
    else {
        printf("Incoming connection: Accepted! \n\n");
    }
    
    // == Application begins here ==

    int counter = 0;
    while(1) {
        counter++;
        printf("\tWEB PROXY - ITERATION %d\n\n", counter);
        printf("Waiting for a new request from the browser\n");

        // Receive HTTP request message from browser
        bzero(net_buff, sizeof(net_buff));
        netBytesRead = read(serverConnFD, net_buff, sizeof(net_buff));
        if (netBytesRead < 0) printf("ERROR when reading from socket\n");

        printf("Received %d bytes from the browser\nThis is the HTTP request:\n", netBytesRead);
        printf("%s\n", net_buff);
        
        // * Start client portion of the proxy *
        // Parse HTTP request to obtain host and url
        // HTTP request has the form:
        // METHOD URL VERSION\r\n
        // Host: HOST\r\n
        // \r\n
        printf("Parsing HTTP request...\n...\n");
        char* url = strtok(net_buff, " ");
        url = strtok(NULL, " ");            // this should the the URL
        char* host = strtok(NULL, " ");
        host = strtok(NULL, "\r");          // this should get the HOST

        // Prepare the getaddrinfo structs and call the function
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo(host, webPort, &hints, &res);

        // Create the client socket
        if((clientSocketFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
            printf("Error when creating the client socket\n");
            return 1;
        }

        // Call the connect() function
        int status;
        if((status = connect(clientSocketFD, res->ai_addr, res->ai_addrlen)) < 0) {
            printf("Error when connecting to web server...\n");
            return 1;
        } else {
            printf("Succesfully connected to the web server!\n");
        }

        // Prepare the HTTP request
        char httpHeader[4096];
        char headerLength;
        bzero(httpHeader, sizeof(httpHeader));

        snprintf(httpHeader, sizeof(httpHeader), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", url, host);
        headerLength = strlen(httpHeader);

        printf("The HTTP request to be send is:\n");
        printf("%s\n", httpHeader);

        // Send the HTTP request to the server
        netBytesWrote = write(clientSocketFD, httpHeader, strlen(httpHeader) + 1);
        if(netBytesWrote < 0) printf("ERROR writing to socket\n");

        // Receive the HTTP response from the web server in chunks and sent it to the browser
        int chunk = 8000;
        bzero(net_buff, sizeof(net_buff));

        while((netBytesRead = read(clientSocketFD, net_buff, chunk)) > 0) {

            printf("Received %d bytes from web server\n", netBytesRead);
            printf("The HTTP response from the web server is:\n");
            printf("%s\n", net_buff);

            // * End of client portion of the proxy *
            // Send the HTTP response to the browser
            netBytesWrote = write(serverConnFD, net_buff, netBytesRead);
            if(netBytesWrote < 0) printf("ERROR writing to browser\n");

            printf("Wrote %d BYTES back to the browser\n\n", netBytesWrote);

            bzero(net_buff, sizeof(net_buff));
        }

        // Close client sockets
        close(clientSocketFD);
        freeaddrinfo(res);

    }   // End of main while loop

    // Close server sockets
    close(serverConnFD);
	close(serverSocketFD);

    return 0;
}



