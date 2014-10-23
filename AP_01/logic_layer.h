#pragma once

#include "application.h"
#include "serial.h"

enum states
{
	START,
	FLAG_RCV,
	A_RCV,
	C_RCV,
	BCC_OK,
	STOP
};

#define F 0x7e
#define A 0x03

#define SET 0x03
#define UA 0x07

int llOpen(struct applicationLayer * appLayer, struct linkLayer * lLayer);
int llwrite(int fd, char * buffer, int length);