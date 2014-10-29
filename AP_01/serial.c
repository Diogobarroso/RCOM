#include "serial.h"

struct termios * oldtio;

int openSerial (char* path)
{
	oldtio = (struct termios *) malloc (sizeof(struct termios));
	struct termios newtio;
	int file_descriptor = open(path, O_RDWR | O_NOCTTY );
	if (file_descriptor < 0)
	{
		perror(path);
		exit(-1);
	}
	if ( tcgetattr(file_descriptor,oldtio) == -1)
	{
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;


	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

	tcflush(file_descriptor, TCIOFLUSH);

	if ( tcsetattr(file_descriptor,TCSANOW,& newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	return file_descriptor;
}

void closeSerial (int file_descriptor, struct termios * oldtio)
{
	if ( tcsetattr(file_descriptor,TCSANOW, oldtio) == -1)
	{
		perror("tcsetattr");
		exit(-1);
	}

	close(file_descriptor);

	printf("Connection closed\n");
}

int writeSerial (char message[], int messageSize, int file_descriptor)
{
	int writeResult = write(file_descriptor, message, messageSize);
	return writeResult;
}

int readSerial (int file_descriptor, char * c)
{
	char buf[1];
	int res;
	int STOP = FALSE;
	while(STOP == FALSE)
	{
		res = read(file_descriptor, buf, 1);

		if(res < 0) 
		{
			perror("Error on read function!\n");
			return (-1);
			//exit(EXIT_FAILURE);
		}
		*c = buf[0];
		return res;
	}



}
