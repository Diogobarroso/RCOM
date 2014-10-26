#pragma once

#include "application.h"
#include "serial.h"

/**
 * @brief Enumeration of the states the State Machine applies
 * @details The State Machine will be applied both for the Client and Server, in order for the program to know what to do/expect next
 * 
 */
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

/**
 * @brief Opens the connection and implements the state machine whether it is called by the Server or the Client
 * @details Opens the connection and implements the State Machine
 * 
 * @param appLayer Structure containing a File Descriptor to the file that is being worked on
 * @param lLayer Structure containing Serial Connection properties
 * 
 * @return 0 on success, -1 otherwise
 */
int llOpen(struct applicationLayer * appLayer, struct linkLayer * lLayer);
int llwrite(int fd, char * buffer, int length);
