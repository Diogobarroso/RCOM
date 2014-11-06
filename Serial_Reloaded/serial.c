#include "serial.h"

struct termios * oldtio;

int baudRate = B38400;

int openSerial (char* path)
{
	oldtio = (struct termios *) malloc (sizeof(struct termios));

	struct termios newtio;
	printf("before open\n");
	int file_descriptor = open (path, O_RDWR | O_NOCTTY);
	printf("after open\n");
	if (file_descriptor < 0)
	{
		perror(path);
		return(-1);
	}

	if (tcgetattr(file_descriptor,oldtio) == -1)
	{
		perror("tcgetattr");
		return(-1);
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = baudRate | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	tcflush(file_descriptor, TCIOFLUSH);

	if(tcsetattr(file_descriptor,TCSANOW,&newtio) == -1)
	{
		perror("tcsetattr");
		return(-1);
	}

	return file_descriptor;
}

void closeSerial (int file_descriptor)
{
	if (tcsetattr(file_descriptor,TCSANOW,oldtio) == -1)
	{
		perror("tcsetattr");
	}

	close(file_descriptor);
}

int writeSerial (int file_descriptor, unsigned char * buffer, int bufferSize)
{
	int writeResult = write(file_descriptor, buffer, bufferSize);
	return writeResult;
}

int readSerial (int file_descriptor, unsigned char * c)
{
printf("Before Read\n");
	int res = read(file_descriptor, c, 1);
printf("After Read character %X\n",*c);
	if (res < 0)
	{
		perror("Timeout!\n");
		return(-1);
	}

	
	return 0;
}

// Configuration

void setBaudRate (int newBaudRate)
{
	baudRate = newBaudRate;
}
