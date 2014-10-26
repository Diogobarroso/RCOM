#include <unistd.h>
#include <signal.h>

#include "application.h"
#include "logic_layer.h"
#include "serial.h"

void alarmHandler(int signo)
{
	printf("Alarm Handler was called, timeout occurred;\n");
}


int main(int argc, char** argv)
{
	/*
	if (argc != 3 && (strcmp("client", argv[2])!=0))
	{
		printf("Usage: %s <port path> <client/server> \n", argv[0] );
		exit (1);
	} else if (argc != 4 && (strcmp("server", argv[2])!=0))
	{
		printf("Usage: %s <port path> <client/server> <file> \n", argv[0] );
		exit (1);
	}
	*/
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
		int fd = llOpen(&appLayer, &lLayer);

		char* startControlPacket = (char*) malloc (23 * sizeof(char));
		readControlPacket(fd, startControlPacket);

		printf("%s\n", startControlPacket);

	}
	else if ((strcmp("server", argv[2])==0))
	{
		// Open the serial
		appLayer.status = SERVER;
		int fd = llOpen(&appLayer, &lLayer);

		writeControlPacket(fd,argv[3]);
		
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

/*
int writeControlPacket(int fd, char* data)
{
	char startControlPacket[23];

	startControlPacket[0] = 2;
	startControlPacket[1] = 2;
	startControlPacket[2] = 20;
	memcpy(startControlPacket + 3, data, 20);

	int writeReturn = writeSerial(startControlPacket, 23, fd);

	if (writeReturn != 23)
		return -1;
	else
		return 1;

}

int readControlPacket(int fd, char * controlPacket)
{

	int index;
	char c;
	for (index = 0; index < 23; index++)
	{
		int r = readSerial(fd, &c);
		if (r != 1)
			exit(1);
		controlPacket[index] = c;

	}

	return 1;
}
*/
