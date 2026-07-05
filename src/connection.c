#include "connection.h"
#include "commands.h"
#include "parsers.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


static pthread_attr_t request_thread_attr;

void connection_init() {
	pthread_attr_init(&request_thread_attr);
	pthread_attr_setdetachstate(&request_thread_attr, PTHREAD_CREATE_DETACHED);
}

void  connection_cleanup() {
	pthread_attr_destroy(&request_thread_attr);
}


static void *client_worker(void *args)
{
	command_context *ctx = calloc(1, sizeof(command_context));
	int client_fd = (int)(intptr_t)args;
	ctx->client_fd = client_fd;

	char buff[1024];

	while (1)
	{
		ssize_t read_bytes = read(client_fd, buff, sizeof(buff) - 1);
		if (read_bytes <= 0) break;

		buff[read_bytes] = '\0';
		resp_value *parsed_resp = parse_resp(buff);
		command_dispatch(ctx, parsed_resp);
		free_resp(parsed_resp);
	}

	close(client_fd);
	free(ctx);

	return NULL;
}

void connection_spawn(int client_fd) {
    pthread_t thread_id;
    int thread_status = pthread_create(&thread_id, &request_thread_attr, &client_worker, (void *)(intptr_t)client_fd);
    if (thread_status != 0) 
    {
        fprintf(stderr, "Failed to open new thread!\n");
        close(client_fd);
    }
}