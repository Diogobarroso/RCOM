#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define F 0x7e
#define A 0x03

#define SET 0x03
#define UA 0x07

struct linkLayer {
	/*Dispositivo /dev/ttySx, x = 0, 1*/
	char port[20];
	/*Velocidade de transmissão*/
	int baudRate;
	/*Número de sequência da trama: 0, 1*/
	unsigned int sequenceNumber;
	/*Valor do temporizador: 1 s*/
	unsigned int timeout;
	/*Número de tentativas em caso de falha*/
	unsigned int numTransmissions;
	/*Trama*/
	char frame[5];
	/*Old Tios*/
	struct termios * oldtio;
};

int openSerial (char* path, struct termios * oldtio);

void closeSerial (int file_descriptor, struct termios * oldtio);

int writeSerial (char message[], int messageSize, int file_descriptor);

char readSerial (int file_descriptor);