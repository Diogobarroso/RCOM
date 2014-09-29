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



void writeSerial(int file_descriptor)
{
	int res;
	char buf[255];
		/*Reads line from STDIN*/
	if(gets(buf) == NULL) 
	{
		printf("ERROR WITH GETS");
		exit(-1);
	}

	int length = (int) strlen(buf) + 1;
	buf[length - 1] = '\0';

	res = write(file_descriptor,buf,length);   
	printf("%d bytes written\n", res);
}

int openSerial (char* path, struct termios* oldtio)
{
	int file_descriptor;
	struct termios newtio;

	file_descriptor = open(path, O_RDWR | O_NOCTTY );
	if (file_descriptor <0)
	{
		perror(path); exit(-1);
	}

	if ( tcgetattr(file_descriptor,oldtio) == -1)
	{ /* save current port settings */
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
	newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

		tcflush(file_descriptor, TCIOFLUSH);

	if ( tcsetattr(file_descriptor,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	printf("New termios structure set\n");

	return file_descriptor;

}

void closeSerial (int fd, struct termios* oldtio)
{
	if ( tcsetattr(fd,TCSANOW,oldtio) == -1)
	{
		perror("tcsetattr");
		exit(-1);
	}

	close(fd);
}

int main(int argc, char** argv)
{
	int fd;
	struct termios oldtio;

	if ( (argc < 2) || ((strcmp("/dev/ttyS1", argv[1])!=0)))
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}


	/*
		Open serial port device for reading and writing and not as controlling tty
		because we don't want to get killed if linenoise sends CTRL-C.
	*/


	fd = openSerial(argv[1], &oldtio);

	writeSerial(fd);
		
	closeSerial(fd, &oldtio);

	return 0;
}
