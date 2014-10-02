/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1



int openSerial(char *path, struct termios* oldtio)
{
	int fd; 
	struct termios newtio;

	fd = open(path, O_RDWR | O_NOCTTY);
	if(fd < 0)
	{
		perror(path);
		exit(EXIT_FAILURE);
	}


	if(tcgetattr(fd, newtio) == -1)
	{
		//Should not happen
		perror("[!] Error on tcgetattr function.");
		exit(EXIT_FAILURE);
	}


	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;

	//Inter-character timer unused
	newtio.c_cc[VTIME] = 0;
	//Blocking read until 5 chars received
	newtio.c_cc[VMIN] = 5;   


	tcflush(fd, TCIOFLUSH);
	if(tcsetattr(fd, TCSANOW, &newtio) == -1) 
	{
		//Should not happen
		perror("[!] Error on tcsetattr function.");
		exit(EXIT_FAILURE);
	}


	printf("New termios structure set\n");
	return fd;
}



void rewriteSerial(char *text, int fd)
{
	int res;
	int length = (int) strlen(text) + 1;

	buf[length - 1] = '\0';
	res = write(fd, text, length);
	printf("%d bytes written\n", res);
}



void readSerial(int fd)
{
	char buf[255];
	char text[1024];
	int STOP = FALSE;
	int currentIndex = 0, res;


	while(STOP == FALSE)
	{
		res = read(fd, buf, 1);
		if(res < 0) 
		{
			//Error
			perror("[!] Error on read function!");
			exit(EXIT_FAILURE);
		}

		text[currentIndex] = buf[0];
		if(text[currentIndex++] == '\0')
			STOP = TRUE;
	}


	printf("Text: %s\n", text);
	rewriteSerial(text, fd);
}



void closeSerial(int fd, struct termios* oldtio)
{
	if(tcsetattr(fd,TCSANOW,oldtio) == -1)
	{
		perror("[!] Error on tcsetattr function.");
		exit(EXIT_FAILURE);
	}

	close(fd);
}


int main(int argc, char** argv)
{
	int fd, res, STOP = 0;
	struct termios oldtio;

	if(argc < 2 || strcmp(MODEMDEVICE, argv[1]) != 0) 
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(EXIT_FAILURE);
	}


	fd = openSerial(argv[1], &oldtio);
	readSerial(fd);
	closeSerial(fd, &oldtio);
	return 0;
}