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
	/*
	struct applicationLayer appLayer;
	struct linkLayer lLayer;
*/
	// Alarm handler setup -----------------
	struct sigaction act;
	act.sa_handler = alarmHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGALRM,&act,NULL) == -1)
		printf("Error in sigaction\n");

	// -------------------------------------


	if ((strcmp("client", argv[2])==0))
	{
		int fd = llOpen(argv[1], CLIENT);

		printf("Serial Port opened\n");

		char* startControlPacket = (char*) malloc (23 * sizeof(char));
		
		int readResult = readControlPacket(fd, C_START, FILE_NAME, 20, startControlPacket);
		
		printf("readResult = %d\n", readResult);
		printf("File Name = %s\n", startControlPacket);

	}
	else if ((strcmp("server", argv[2])==0))
	{
		int fd = llOpen(argv[1], SERVER);

		printf("Serial Port opened\n");

		writeControlPacket(fd, C_START, FILE_NAME, 20, argv[3]);
		
		/*
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

			// Send file Packet ----------------------

		}
		printf("File is %d bytes long\n", total);
*/
	}
	else 
	{
		printf("Usage: %s <port path> <client/server>\n", argv[0] );
		exit (1);
	}
}

int writeControlPacket(int fd, int c, int t, int l, char* data)
{
	char * controlPacket = (char*) malloc ((3 + l) * sizeof (char));
	controlPacket[0] = c;
	controlPacket[1] = t;
	controlPacket[2] = l;
	memcpy(controlPacket + 3, data, l);

	printf("Control Packet:\n\tC->%d\n\tT->%d\n\tL->%d\n",controlPacket[0],controlPacket[1],controlPacket[2]);
	int index = 0;
	for (index = 0; index < l; index++)
	{
		printf("\tdata[%d]->%c\n", index, controlPacket[index + 3]);
	}

	int writeReturn = llwrite(fd, controlPacket, 3 + l);

	if (writeReturn != (3 + l))
		return -1;
	else
		return 0;
}

int readControlPacket(int fd, int c, int t, int l, char* data)
{

	printf("readControlPacket(%d, %d, %d, %d, data)\n", fd, c, t, l);

	char * controlPacket = (char*) malloc ((3 + l) * sizeof (char));

	int readReturn = llread (fd, controlPacket);

	if (controlPacket[0] != c)
	{
		printf("controlPacket[0] = %d\n", controlPacket[0]);
		return -1;
	}
	if (controlPacket[1] != t)
	{
		printf("controlPacket[1] = %d\n", controlPacket[1]);
		return -1;
	}
	if (controlPacket[2] != l)
	{
		printf("controlPacket[2] = %d\n", controlPacket[2]);
		return -1;
	}

	printf("Data Size = %d\n", sizeof (data));
	memcpy(data, controlPacket + 3, l);

	return 0;
}
