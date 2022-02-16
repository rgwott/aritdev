/**
 * \file client.c
 *
 * \date 2022/02/15
 * \author Rodolfo Wottrich
 * \version 1.0
 *
 * \brief A userspace application that requests basic arithmetic operations to
 *        to server application and reads back a success code and the result.
 *
 */


// Standard header files
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Application header files
#include "client.h"

void
print_menu(void)
{
	printf("(1) Add 2 numbers\n");	
	printf("(2) Multiply 2 numbers\n");	
	printf("(3) Subtract 2 numbers\n");	
	printf("(4) Divide 2 numbers\n");	
	printf("(5) Exit\n");	
}

void
read_input(uint8_t *command, int64_t *operand1, int64_t *operand2)
{
	int ret;

	printf("Enter command: ");
	ret = scanf("%hhd", command);
	if(ret != 1 || *command == 0 || *command > 5)
	{
		printf("Invalid input.\n");
		exit(EXIT_FAILURE);
	}

	if(*command == 5)
	{
		printf("Exiting...\n");
		exit(EXIT_SUCCESS);
	}

	printf("Enter operand 1: ");
	scanf("%ld", operand1);
	if(ret != 1)
	{
		printf("Invalid input.\n");
		exit(EXIT_FAILURE);
	}

	printf("Enter operand 2: ");
	scanf("%ld", operand2);
	if(ret != 1)
	{
		printf("Invalid input.\n");
		exit(EXIT_FAILURE);
	}
}

int
main(void)
{
	int socket_fd;
	struct sockaddr_in server;
	uint8_t command;
	uint8_t msg[REQUEST_LEN];
	uint8_t response;
	int64_t operand1;
	int64_t operand2;
	uint64_t result;

	print_menu();

	// Read input and perform validations
	read_input(&command, &operand1, &operand2);

	// Set corresponding operation ASCII symbol
	switch(command)
	{
		case 1:
			command = '+';
			break;

		case 2:
			command = '*';
			break;

		case 3:
			command = '-';
			break;

		case 4:
			command = '/';
			break;

		default: // Should not be able to get here
			printf("Invalid command.\n");
			exit(EXIT_FAILURE);
	}

	// Assemble request
	msg[0] = command;
	memcpy(&msg[1], &operand1, sizeof(operand1));
	memcpy(&msg[9], &operand2, sizeof(operand2));

	// Open socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1)
	{
		printf("Could not create socket");
	}

	server.sin_addr.s_addr = inet_addr(SERVER_HOST);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	// Connect to server
	if(connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	printf("Sending request...\n");

	// Send request
	if(send(socket_fd, msg, REQUEST_LEN, 0) < 0)
	{
		puts("Send failed");
		return 1;
	}

	// Read response with status code
	if(recv(socket_fd, &response, sizeof(response), 0) < 0)
	{
		puts("recv failed");
	}

	switch(response)
	{
		case SUCCESS:
			printf("Request OKAY...\n");
			break;

		case EINVLEN:
			printf("Request was invalid (invalid length).\n");
			close(socket_fd);
			exit(EXIT_FAILURE);

		case EINVOP:
			printf("Request was invalid (invalid operation).\n");
			close(socket_fd);
			exit(EXIT_FAILURE);

		default:
			printf("Request was invalid (unknown error).\n");
			close(socket_fd);
			exit(EXIT_FAILURE);
	}	

	printf("Receiving result...\n");

	// We are here because request was successful. Read result
	if(recv(socket_fd, &result, sizeof(result), 0) < 0)
	{
		puts("recv failed");
	}

	printf("Result is %ld.\n", result);

	close(socket_fd);

	exit(EXIT_SUCCESS);
}
