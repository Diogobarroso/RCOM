#include <unistd.h>
#include <signal.h>

#include "application.h"
#include "logic_layer.h"
#include "serial.h"

void alarmHandler(int signo)
{
  printf("ALRAM HANDLER\n");
}


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <port path> <client/server>\n", argv[0] );
		exit (1);
	}
	struct applicationLayer appLayer;
	struct linkLayer lLayer;

	struct sigaction act;
    act.sa_handler = alarmHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGALRM,&act,NULL) == -1)
      printf("Erro a fazer sigaction\n");

	strncpy(lLayer.port, argv[1],20);
	lLayer.oldtio = (struct termios *) malloc (sizeof(struct termios));

	if ((strcmp("client", argv[2])==0))
	{
		appLayer.status = CLIENT;
		llOpen(&appLayer, &lLayer);

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