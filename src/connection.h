#ifndef CONNECTION_H
#define CONNECTION_H

void connection_init(void);
void connection_cleanup(void);
void connection_spawn(int client_fd);

#endif 