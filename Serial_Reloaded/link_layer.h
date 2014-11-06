#pragma once

#include "application.h"
#include "serial.h"
#include "utils.h"

// Macros

#define 	FLAG 		0x7E
#define		ADDRESS		0x03
#define		C0			0x00
#define		C1			0x80

#define		SET			0x03
#define		DISC		0x0B
#define		UA			0x08
#define		RR0			0x05
#define		RR1			0x85
#define		REJ0		0x01
#define		REJ1		0x81

#define 	ESCAPE		0x7D
#define		STUFCHAR	0x20

#define		IMH_SIZE	4
#define		IMT_SIZE	2

// State Machine States

int global_mode;

enum states
{
	START,
	FLAG_RCV,
	A_RCV,
	C_RCV,
	BCC_OK,
	STOP
};

// Public

int llOpen(unsigned char* port, int mode);

int llClose(int file_descriptor);

int llWrite(int file_descriptor, unsigned char* packet, int length);

int llRead(int file_descriptor, unsigned char* packet);

// Private

int writeSupervisionMessage(int file_descriptor, unsigned char controlField);

int readSupervisionMessage(int file_descriptor, unsigned char * controlField);

int writeInformationMessage(int file_descriptor, unsigned char * packet, int packetSize);

int readInformationMessage(int file_descriptor, unsigned char * packet);

unsigned char calculateParity (unsigned char* array, int length);

void switchSequenceNumber ();

int byteStuffing(unsigned char* array, unsigned char** stuffed_message, int length);

int byteDeStuffing(unsigned char * array,unsigned char **destufed_message, int length);
