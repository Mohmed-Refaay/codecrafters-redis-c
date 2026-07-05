
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int server_fd;

void server_init() {
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		fprintf(stderr, "Socket creation failed: %s...\n", strerror(errno));
        exit(1);
	}

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		fprintf(stderr, "SO_REUSEADDR failed: %s \n", strerror(errno));
        exit(1);
	}

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(6379),
		.sin_addr = {htonl(INADDR_ANY)},
	};

	if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
		fprintf(stderr, "Bind failed: %s \n", strerror(errno));
        exit(1);
	}

	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		fprintf(stderr, "Listen failed: %s \n", strerror(errno));
        exit(1);
	}

}

void server_run(void (*spawn_client_handler)(int)){
	socklen_t client_addr_len;
	struct sockaddr_in client_addr;

	printf("Waiting for a client to connect...\n");

	client_addr_len = sizeof(client_addr);

	while (1) {
		int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
		if (client_fd == -1)
		{
			fprintf(stderr,"connection failed: %s...\n", strerror(errno));
			continue;
		}
		printf("Client connected: %d\n", client_fd);

		spawn_client_handler(client_fd);
	}
}

void server_cleanup() {
    close(server_fd);
}