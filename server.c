#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "admin.h"
#include "proffesor.h"
#include "student.h"
#define PORT 49152 
#define BUFFER_SIZE 1024
// Global variable to track if the server should keep running

void handleClient(int clientSocket);

int main(int argc ,char *argv[])
{
	if(argc!=2){
		perror("Enter the port as well after the exe name");
		exit(1);
	}
	int serverSocket, clientSocket;
	struct sockaddr_in serverAddress, clientAddress;
	socklen_t clientAddrLen = sizeof(clientAddress);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		perror("Socket creation failed");
		exit(1);
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(argv[1]));
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("Socket binding failed");
		exit(1);
	}

	if (listen(serverSocket, 5) == -1)
	{
		perror("Listening failed");
		exit(1);
	}

	printf("Server is listening on port %s...\n", argv[1]);
	

	while (1)
	{
		int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
		if (clientSocket == -1)
		{
			perror("Accepting connection failed");
			continue;
		}

		pid_t childPID = fork();
		if (childPID < 0)
		{
			perror("Fork failed");
		}
		else if (childPID == 0)
		{
			// This is the child process
			close(serverSocket); // Close the server socket in the child process
			handleClient(clientSocket);
			close(clientSocket);
			exit(0); // Terminate the child process
		}
		else
		{
			// This is the parent process
			close(clientSocket);		// Close the client socket in the parent process
			waitpid(-1, NULL, WNOHANG); // Cleanup zombie child processes
		}
	}

	// Close the server socket (never reached in this example)
	close(serverSocket);
	return 0;
}

void handleClient(int clientSocket)
{
	char buffer[BUFFER_SIZE];
	int bytesRead;

	// Prompt the client to choose a role
	char rolePrompt[] = "\nSelect your role:\n1. Admin\n2. Professor\n3. Student\n4.Exit the client";
	while (1)
	{
		memset(buffer,0,sizeof(buffer));
		send(clientSocket, rolePrompt, strlen(rolePrompt), 0);

		// Receive the client's role choice
		bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead <= 0)
		{
			perror("Error receiving role choice");
			close(clientSocket);
			return;
		}
		buffer[bytesRead] = '\0';
		// Process the client's role choice
		int choice=atoi(buffer);
		switch (choice)
		{
		case 1:
			// Handle Admin role
			// Implemented the admin-related functionality here
			char response[] = "<-----------Welcome, Admin!---------->\n";
			send(clientSocket, response, strlen(response), 0);
			if(!admin_functionality(clientSocket))break;
				break;
		case 2:
			// Handle Professor role
			// Implemented the professor-related functionality here
			char response1[] = "<-----------Welcome, Professor!----------->\n";
			send(clientSocket, response1, strlen(response1), 0);
			if(!proffesor_functionality(clientSocket))break;
				break;
		case 3:
			// Handle Student role
			// Implemented the student-related functionality here
			char response2[] = "<-----------Welcome, Student!----------->\n";
			send(clientSocket, response2, strlen(response2), 0);
			if(!student_functionality(clientSocket))break;
				break;
		default:
			char response3[] = "You are Exiting Now !!.\n";
			send(clientSocket, response3, strlen(response3), 0);
			printf("Client gets Exited\n");
			close(clientSocket);
			return;
		}
		/*
		if (buffer[0] == '1')
		{
			// Handle Admin role
			// Implemented the admin-related functionality here
			char response[] = "<-----------Welcome, Admin!---------->\n";
			send(clientSocket, response, strlen(response), 0);
			if(!admin_functionality(clientSocket))continue;
		}
		else if (buffer[0] == '2')
		{
			// Handle Professor role
			// Implemented the professor-related functionality here
			char response[] = "<-----------Welcome, Professor!----------->\n";
			send(clientSocket, response, strlen(response), 0);
			if(!proffesor_functionality(clientSocket))continue;
		}
		else if (buffer[0] == '3')
		{
			// Handle Student role
			// Implemented the student-related functionality here
			char response[] = "<-----------Welcome, Student!----------->\n";
			send(clientSocket, response, strlen(response), 0);
			if(!student_functionality(clientSocket))continue;
		}
		else
		{
			char response[] = "You are Exiting Now !!.\n";
			send(clientSocket, response, strlen(response), 0);
			printf("Client gets Exited\n");
			close(clientSocket);
			return;
		*/
	}
	// Close the client socket
	close(clientSocket);
}
