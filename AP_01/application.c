#include <unistd.h>
#include <signal.h>

#include "application.h"
#include "logic_layer.h"
#include "serial.h"


int sequencePacketNumber;


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
	
	// Alarm handler setup -----------------
	struct sigaction act;
	act.sa_handler = alarmHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGALRM,&act,NULL) == -1)
		printf("Error in sigaction\n");

	// -------------------------------------

	sequencePacketNumber = 0;

	if ((strcmp("client", argv[2])==0))
	{
		int fd = llOpen(argv[1], CLIENT);

		printf("Serial Port opened\n");

		char* startControlPacket = (char*) malloc (23 * sizeof(char));
		char* packet = (char*) malloc (PACKET_SIZE * sizeof(char));
		
		int readResult = readPacket(fd, startControlPacket);
		do
		{
			readResult = readPacket(fd, packet);
			if (readResult < 0)
				sendSuperPacket(fd,REJ);
			else
				sendSuperPacket(fd,RR);
			
		} while(readResult != 2);
		
		printf("readResult = %d\n", readResult);
		printf("File Name = %s\n", startControlPacket);

	}
	else if ((strcmp("server", argv[2])==0))
	{
		int fd = llOpen(argv[1], SERVER);

		printf("Serial Port opened\n");

		writeControlPacket(fd, C_START, FILE_NAME, 20, argv[3]);
		
		
		// Read file
		FILE* fr = fopen(argv[3], "rb");

		char buffer[PACKET_SIZE];

		int bytesRead = 0, total = 0, index = 0, tries = 0;
		
		while (!feof(fr))
		{
			bytesRead = fread(buffer, 1, PACKET_SIZE, fr);
			printf("bytesRead = = = = = = = = = = = = = = = = = = = = %u", bytesRead);
			if (ferror (fr))
			{
				perror("Read Error");
				exit(1);
			}
				
			total += bytesRead;

			writeInfoPacket(fd, bytesRead, buffer);

			while (tries < 3)
			{
				printf("Reading\n");
				if(readSuperPacket(fd) == 0)
				{	
					tries = 0;
					switchSequenceNumber();
					break;
				}
				else
				{
					tries++;
					writeInfoPacket(fd,bytesRead,buffer);
				}
			}


		}

		printf("File is %d bytes long\n", total);

		writeControlPacket(fd, C_END, FILE_SIZE, sizeof(int), &total);

	}
	else 
	{
		printf("Usage: %s <port path> <client/server>\n", argv[0] );
		exit (1);
	}
}

int writeControlPacket(int fd, int c, int t, int l, unsigned char* data)
{
	unsigned char * controlPacket = (unsigned char*) malloc ((3 + l) * sizeof (unsigned char));
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
	{
		switchSequenceNumber();
		return 0;
	}
}

int writeInfoPacket(int fd, int length, unsigned char* data)
{
	unsigned char * infoPacket = (unsigned char*) malloc ((PACKET_SIZE + 4) * sizeof (unsigned char));
	infoPacket[0] = 1;
	infoPacket[1] = (++sequencePacketNumber) % 255;
	infoPacket[2] = length / 255;
	int resto = length % 255;
	printf("Resto = %u\n",resto);
	infoPacket[3] = resto;
	memcpy(infoPacket + 4, data, length);
	
	printf("writeInfoPacket:\n");
	int i;
	for (i = 0; i < length + 4; i++)
	{
		printf("infoPacket[%u] = %X | %c\n",i,infoPacket[i],infoPacket[i]);
	}

	int writeReturn = llwrite(fd, infoPacket, length + 4);

	if(writeReturn != (length + 4))
		return -1;
	else
		return 0;
}


int readControlPacket(unsigned char* buffer, unsigned char* data, int b_length)
{
	printf("Data Size = %d\n", sizeof (data));
	memcpy(data, buffer + 3, b_length - 3);

	return 0;
}


int readInfoPacket(unsigned char * buffer,unsigned char* data)
{
	memcpy(data,buffer + 4, PACKET_SIZE - 4);

	return 0;
}

int readPacket(int fd, unsigned char* data)
{
	unsigned char * buffer = (unsigned char *) malloc (PACKET_SIZE + 5);
	
	llread(fd, buffer);
	
	printf("buffer[0] = %X\n", buffer[0]);
	
	if(buffer[0] == C_INFO)
	{
		printf("INFO\n");
		readInfoPacket(buffer, data);
		switchSequenceNumber();		
		sequencePacketNumber++;
		return 0;
	}
	else if(buffer[0] == C_START)
	{
		printf("START\n");
		readControlPacket(buffer, data, 20);
		sequencePacketNumber++;
		return 1;
	}
	else if (buffer[0] == C_END)
	{
		printf("END\n");
		readControlPacket(buffer, data, sizeof(int));
		sequencePacketNumber++;
		return 2;
	} else {
		printf("FAIL\n");
		return -1;
	}	
}
