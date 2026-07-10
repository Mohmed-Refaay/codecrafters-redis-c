#include <stdio.h>
#include "connection.h"
#include "server.h"
#include "store.h"


int main()
{
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// init
	connection_init();
	store_init();
	server_init();

	// start_server
	server_run(connection_spawn);

	return 0;
}
