#ifndef SERVER_H
#define SERVER_H

void server_init(void);
void server_run(void (* spawn_client_handler)(int client_fd));
void server_cleanup(void);

#endif