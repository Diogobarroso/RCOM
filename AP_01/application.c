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

	sequencePacketNumber = 0;

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
			total += fread(dataPacket, 1, PACKET_SIZE, fr);
printf("Going to fread\n");
			writeInfoPacket(fd, PACKET_SIZE, dataPacket);

		}
		printf("File is %d bytes long\n", total);

		writeControlPacket(fd, C_END, FILE_SIZE, PACKET_SIZE, &total);

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

int writeInfoPacket(int fd, int length, char* data)
{

	//char * infoPacket = (char*) malloc ((PACKET_SIZE) * sizeof (char));
	data[0] = 1;
	data[1] = (++sequencePacketNumber) % 255;
	data[2] = PACKET_SIZE / 256;
	data[3] = PACKET_SIZE % 256;

	int writeReturn = llwrite(fd, data, length);

	if(writeReturn != (length))
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

int readInfoPacket(int fd, char* data)
{
	int i = 0;
	float maxI = sizeof(char)/sizeof(char[0]);

	printf("Info Packet read, it is a ");
	if(data[1] == (sequencePacketNumber +1))
	{
		switchSequenceNumber();

		char * infoPacket = (char*) malloc (PACKET_SIZE * sizeof (char));

		int readReturn = llread (fd, infoPacket); //Already read in the function below??


		if(infoPacket[1] == C_START && infoPacket[2] == FILE_NAME)
		{
			printf("Start Packet\n");
			for(; i < maxI; ++i)
				printf("startPacket[%u]: %s\n", i, data[i]);
			
		}
		else if(infoPacket[1] == C_END && infoPacket[2] == FILE_SIZE)
		{
			printf("End Packet\n");
			for(; i < maxI; ++i)
				printf("endPacket[%u]: %s\n", i, data[i]);
		}
		else
		{
			printf("Problem reading Info Packet.\nRejecting...\n");
			char * rej = (char *) malloc (5 * sizeof(char));
			rej[0] = 1;
			rej[1] = (sequencePacketNumber + 1) % 256;
			rej[2] = 129;
			rej[3] = calculateParity(&rej[0], 3);
			rej[4];// <----------------------
			return -1;
		}

		memcpy(data,infoPacket + 3, PACKET_SIZE - 3);
		char * rr = (char *) malloc (5 * sizeof(char));
		rr[0] = 1;
		rr[1] = (sequencePacketNumber + 1) % 256;
//		if(sequenceNumber)
//			rr[2] = 134;
//		else
			rr[2] = 5;
		rr[3] = calculateParity(&rr[0], 3);
		rr[4]; //<-------------------------
	}
	else
	{
		printf("Error in the sequence\nRejecting...\n");
		char * rej = (char *) malloc (5 * sizeof(char));
		rej[0] = 1;
		rej[1] = (sequencePacketNumber + 1) % 256;
//		if(sequenceNumber)
//			rej[2] = 134;
//		else
			rej[2] = 5;
		rej[3]; // <----------------------
		return -1;
	}

	sequencePacketNumber++;
	return 0;
		
}

int readPacket(int fd, char* data)
{
	int success;
	char * buffer = llread(fd, buffer);
	if(buffer[0] == C_INFO)
		success = readInfoPacket(fd, data);
	else if(buffer[0] == C_START || buffer[0] == C_END)
		success = readControlPacket(fd, buffer[0], buffer[1], buffer[2], buffer[3]);

	return success;
}
