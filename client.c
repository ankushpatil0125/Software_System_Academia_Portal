
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 49152

int main(int argc,char *argv[]) {
    if(argc!=2){
		perror("Enter the port as well after the exe name");
		exit(1);
	}
    int clientSocket;
    struct sockaddr_in serverAddress;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[1]));
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Connection failed");
        close(clientSocket);
        exit(1);
    }
    printf("You are Now Connected to the server\n");
    while (1) {
        // Receive data from the server
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            perror("Error receiving data");
            break;
        }
        // Process and display received data
        printf(">%s", buffer);
        // Send a response (optional)
        char message[1024];
        printf("\n>");
        memset(message, 0, sizeof(message));
        fgets(message, sizeof(message), stdin);
        message[strlen(message)-1]='\0';
        send(clientSocket, message, strlen(message), 0);
    }
    // Close the socket
    close(clientSocket);

    return 0;
}

