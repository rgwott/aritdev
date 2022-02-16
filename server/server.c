/**
 * \file server.c
 *
 * \date 2022/02/15
 * \author Rodolfo Wottrich
 * \version 1.0
 *
 * \brief A userspace application that listens to requests on a socket,
 *        forwards them to the aritdev character device, retrieves the
 *        result of the arithmetic operation, and responds to the client.
 *
 */


// Standard header files
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Application header files
#include "server.h"

int
main(void)
{
	int c;
	int chardev_fd;
	int new_socket_fd;
	int ret;
	int socket_fd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	uint8_t operation;
	uint8_t request[REQUEST_LEN];
	uint8_t response;
	int64_t operand1;
	int64_t operand2;
	uint64_t result;

	// Open aritdev character device
	chardev_fd = open(ARITDEV_PATH, O_RDWR);
	if(chardev_fd == -1)
	{
		perror("open /dev/aritdev");
		exit(EXIT_FAILURE);
	}

	// Create socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1)
	{
		perror("Could not create socket\n");
		exit(EXIT_FAILURE);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(LISTEN_PORT);

	// Bind socket to port 8080
	if(bind(socket_fd, (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Bind failed\n");
	}

	// Listen for requests
	listen(socket_fd, 3);

	printf("Server is listening to requests.\n");

	c = sizeof(struct sockaddr_in);

	while((new_socket_fd = accept(socket_fd, (struct sockaddr *)&client,
	                              (socklen_t*)&c)))
	{
		printf("Client connected.\n");

		// When a client connects, read the request
		if((ret = recv(new_socket_fd, request, REQUEST_LEN, 0)) < 0)
		{
			puts("recv failed");
		}

		// Validate request length
		if(ret != REQUEST_LEN)
		{
			printf("Received request of invalid length.\n");
			// Respond with error code
			response = EINVLEN;
			write(new_socket_fd, &response, sizeof(response));
			continue;
		}

		operation = request[0];

		// Validate requested operation
		if(operation != '+' && operation != '-' &&
		   operation != '*' && operation != '/')
		{
			printf("Received request for invalid operation.\n");
			// Respond with error code
			response = EINVOP;
			write(new_socket_fd, &response, sizeof(response));
			continue;
		}

		memcpy(&operand1, &request[1], sizeof(operand1));
		memcpy(&operand2, &request[9], sizeof(operand2));

		printf("Received request for %ld %c %ld.\n", operand1, operation, operand2);

		// Respond with success code
		response = SUCCESS;
		write(new_socket_fd, &response, sizeof(response));

		// Request operation to aritdev
		ret = write(chardev_fd, request, REQUEST_LEN);
		if(ret == -1)
		{
			perror("Error");
			exit(-1);
		}

		// Read result from aritdev
		ret = read(chardev_fd, &result, sizeof(result));
		if(ret == -1)
		{
			perror("reading /dev/aritdev");
		}
		else
		{
			// Send result to client
			printf("Sending result.\n");
			write(new_socket_fd, &result, sizeof(result));
		}

		close(new_socket_fd);
	}

	if(new_socket_fd < 0)
	{
		perror("accept failed");
		return 1;
	}

	close(chardev_fd);
	close(socket_fd);

	exit(EXIT_SUCCESS);
}
