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
	if ((strcmp("client", argv[2])==0) && argc != 3)
	{
		printf("Usage: %s <port path> <client/server> \n", argv[0] );
		exit (1);
	} else if ((strcmp("server", argv[2])==0) && argc != 4)
	{
		printf("Usage: %s <port path> <client/server> <file> \n", argv[0] );
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


		// Open the serial
		appLayer.status = SERVER;
		int fd = llOpen(&appLayer, &lLayer);

		char startControlPacket[23];

		startControlPacket[0] = '2';
		startControlPacket[1] = '2';
		startControlPacket[2] = 20 ;
		memcpy(startControlPacket + 3, argv[3], 20);

		int writeReturn = writeSerial(startControlPacket, 23, fd);

		printf("%d\n", writeReturn);

		// Read file
		FILE* fr = fopen(argv[3], "rb");

		char buffer[PACKET_SIZE];
		char dataPacket[4+PACKET_SIZE];
		int bytesRead = 0, total = 0, index = 0;

		while (!feof(fr))
		{
			bytesRead = fread(buffer, 1, PACKET_SIZE, fr);
			if (ferror (fr))
			{
				perror("Read Error");
				exit(1);
			}
			total += bytesRead;

			// Send file Packet

		}
		printf("File is %d bytes long\n", total);

	}
	else 
	{
		printf("Usage: %s <port path> <client/server>\n", argv[0] );
		exit (1);
	}
}