#include <unistd.h>

#include "application.h"
#include "logic_layer.h"
#include "serial.h"

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <port path> <client/server>\n", argv[0] );
		exit (1);
	}
	struct applicationLayer appLayer;
	struct linkLayer lLayer;

	strncpy(lLayer.port, argv[1],20);
	lLayer.oldtio = (struct termios *) malloc (sizeof(struct termios));

	if ((strcmp("client", argv[2])==0))
	{
		appLayer.status = CLIENT;

	}
	else if ((strcmp("server", argv[2])==0))
	{
		appLayer.status = SERVER;
		llOpen(&appLayer, &lLayer);
	}
	else 
	{
		printf("Usage: %s <port path> <client/server>\n", argv[0] );
		exit (1);
	}
}