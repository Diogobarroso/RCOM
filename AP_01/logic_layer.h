#pragma once

#include "application.h"
#include "serial.h"
#include "stdio.h"

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

#define RR 0x10
#define RR0 0x05
#define RR1 0x85

#define REJ 0x11
#define REJ0 0x01
#define REJ1 0x81

#define C0 0x00
#define C1 0x40

#define ESCAPE 0x7d
#define STUFCHAR 0x20


/**
 * @brief Opens the connection through the Serial Port
 * @details Uses an elaborated algorithm to open the connection, involving voodoo dolls and stuff
 * If you really want to know how it works your best move would be to read the code
 * 
 * @param port Path to the Serial Port
 * @param mode Usage mode, SERVER or CLIENT
 * 
 * @return File Descriptor to the Serial Port
 */
int llOpen(unsigned char * port, int mode);

/**
 * @brief Write to the Serial Port
 * @details Receives a Packet from the Application Layer, adds a header and a Data Protection Field and sends it to the physical layer
 * The resulting array has the following structure: F A C BCC1 D1 ... Dn BCC2 F
 * F - Flag
 * A - Address
 * C - Control
 * BCC1 - Protection for the Header
 * D1 ... Dn - Data bytes
 * BCC2 - Protection for the Data
 * F - Flag
 * 
 * @param fd File Descriptor of the Serial Port
 * @param buffer Data bytes
 * @param length Length of the buffer (in bytes)
 * @return Number of bytes written on success, -1 on error
 */
int llwrite(int fd, unsigned char * buffer, int length);

/**
 * @brief Reads from the Serial Port
 * @details Receives a chunk of data from the Serial Port, processes it as an Information sequence with a state machine
 * 
 * 
 * @param fd File Descriptor of the Serial Port
 * @param buffer Data bytes
 * 
 * @return Size of the Array on success, -1 on error
 */
int llread(int fd, unsigned char * buffer);

/**
 * @brief Calculates parity of array
 * @details Runs through array to calculate its parity and return its value
 * 
 * @param array Array to be parsed
 * @param length Length of the array to be parsed
 * 
 * @return Result of the parity check
 */
unsigned char calculateParity (unsigned char* array, int length);

/**
 * @brief Switches the Sequence Number
 * @details If the Sequence Number is 1 switches to 0 and vice-versa
 */
void switchSequenceNumber ();

/**
 * @brief Stuffs the byte array
 * @details [long description]
 * 
 * @param array [description]
 * @param length [description]
 * 
 * @return [description]
 */
int byteStuffing(unsigned char* array, char* stuffed_message, int length);

/**
 * @brief Destuffs the byte array
 * @details [long description]
 * 
 * @param array [description]
 * @param length [description]
 * 
 * @return [description]
 */
int byteDeStuffing(unsigned char * array, char *destufed_message, int length);

int sendSuperPacket(int fd,unsigned  char c);
