#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

void *handle_request(void *args)
{
	int client_fd = (int)(intptr_t)args;

	char buff[1024];
	char *response = "+PONG\r\n";

	while (1)
	{
		ssize_t read_bytes = read(client_fd, buff, sizeof(buff));
		if (read_bytes <= 0)
		{
			break;
		}
		write(client_fd, response, strlen(response));
	}

	close(client_fd);

	pthread_exit(NULL);
}

int main()
{
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// setup the attrs of the thread which handles requests
	pthread_attr_t request_thread_attr;
	pthread_attr_init(&request_thread_attr);
	pthread_attr_setdetachstate(&request_thread_attr, PTHREAD_CREATE_DETACHED);

	// Uncomment the code below to pass the first stage
	int server_fd;
	socklen_t client_addr_len;
	struct sockaddr_in client_addr;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		fprintf(stderr, "Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		fprintf(stderr, "SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(6379),
		.sin_addr = {htonl(INADDR_ANY)},
	};

	if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
	{
		fprintf(stderr, "Bind failed: %s \n", strerror(errno));
		return 1;
	}

	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0)
	{
		fprintf(stderr, "Listen failed: %s \n", strerror(errno));
		return 1;
	}

	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);

	while (1)
	{
		int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
		if (client_fd == -1)
		{
			fprintf(stderr,"connection failed: %s...\n", strerror(errno));
			continue;
		}
		printf("Client connected: %d\n", client_fd);

		pthread_t thread_id;
		int thread_status = pthread_create(&thread_id, &request_thread_attr, &handle_request, (void *)(intptr_t)client_fd);
		if (thread_status != 0)
		{
			fprintf(stderr, "Failed to open new thread!\n");
			close(client_fd);
		}
	}

	// cleanup
	pthread_attr_destroy(&request_thread_attr);
	close(server_fd);

	return 0;
}
