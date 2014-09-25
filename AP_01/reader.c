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
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


int main(int argc, char** argv)
{
	int fd,c = 0, res, STOP = 0;
	struct termios oldtio,newtio;
	char buf[255], text[1024];


	if ( (argc < 2) || 
		((strcmp("/dev/ttyS1", argv[1])!=0))) {
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
	exit(1);
}


	/*
		Open serial port device for reading and writing and not as controlling tty
		because we don't want to get killed if linenoise sends CTRL-C.
	*/

		/* Error Testing */
		fd = open(argv[1], O_RDWR | O_NOCTTY );
		if (fd <0) {perror(argv[1]); exit(-1); }

		/* Get old termios */
		if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
		perror("tcgetattr");
		exit(-1);
	}

		/* Reset New Termios */
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

		/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

		newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
		newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



	/* 
		VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
		leitura do(s) próximo(s) caracter(es)
	*/



		tcflush(fd, TCIOFLUSH);

		if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
			perror("tcsetattr");
			exit(-1);
		}

		printf("New termios structure set\n");

		
		int i = 0;

	while (STOP==FALSE) {       /* loop for input */
		res = read(fd,buf,1);   /* returns after at least 5 chars have been input */
		text[i] = buf[0];
		//printf("%u - %c\n", i, text[i]);
		if (text[i++] == '\0') {
			STOP = TRUE;
		}
	}

	printf("\n\nCount = %u\n" , i);
	printf("Text = %s\n" , text);


	/* 
		O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
	*/



		tcsetattr(fd,TCSANOW,&oldtio);
		close(fd);
		return 0;
	}