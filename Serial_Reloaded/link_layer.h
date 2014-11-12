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
/**
 * @brief Prepares the Link
 * @details Prepares the Link to another computer through the Serial Port indicated by port
 * 
 * @param port Indicates the path to the Serial Port
 * @param mode Indicates if the Link will be open as a Client (receiver) or Server (emitter)
 * 
 * @return 0 on success, negative otherwise
 */
int llOpen(unsigned char* port, int mode);

/**
 * @brief Closes the Link
 * @details Closes the Link to another computer
 * 
 * @param file_descriptor Indicates the path to the Serial Port whose connection will be closed
 * @return 0 on success, negative otherwise
 */
int llClose(int file_descriptor);

/**
 * @brief Prepares the packet to be written in the Serial Port
 * @details Prepares the packet to be written in the Serial Port by adding a Header and a Tail and Stuffing it
 * 
 * @param file_descriptor Indicates the path to the Serial Port
 * @param packet The packet that will be written in the Serial Port
 * @param length The length of the packet that will be written in the Serial Port
 * @return 0 on success, negative otherwise
 */
int llWrite(int file_descriptor, unsigned char* packet, int length);

/**
 * @brief Prepares the packet to be read by the Application Layer
 * @details Prepares the packet received through the Serial Port indicated by file_descriptor to be read by the Application Layer
 * 
 * @param file_descriptor Indicates the path to the Serial Port
 * @param packet The char pointer where the info read from the Serial Port will be written
 * 
 * @return The number of chars read from the Serial Port
 */
int llRead(int file_descriptor, unsigned char* packet);

// Private

/**
 * @brief Writes a Control Packet to the Serial Port
 * @details Writes a Control Packet to the Serial Port indicated by file_descriptor
 * 
 * @param file_descriptor Indicates the path to the Serial Port
 * @param controlField Has the Control Field that will be written to the Serial Port
 * 
 * @return 0 on success, negative otherwise
 */
int writeSupervisionMessage(int file_descriptor, unsigned char controlField);

/**
 * @brief Reads a Control Packet from the Serial Port
 * @details Reads a Control Packet from the Serial Port indicated by file_descriptor, and writes into the controlField
 * 
 * @param file_descriptor Indicates the path to the Serial Port
 * @param controlField The char pointer where the packet read from the Serial Port will be written
 * 
 * @return The size of the packet read
 */
int readSupervisionMessage(int file_descriptor, unsigned char * controlField);

/**
 * @brief Writes an Info Packet to the Serial Port
 * @details Writes an Info Packet to the Serial Port
 * 
 * @param file_descriptor Indicates the path to the Serial Port
 * @param packet The char pointer that holds the information to be written
 * @param packetSize The size of the info held in packet
 * @return 0 on success, negative otherwise
 */
int writeInformationMessage(int file_descriptor, unsigned char * packet, int packetSize);

/**
 * @brief Reads an Info Packet from the Serial Port
 * @details Reads an Info Packet from the Serial Port indicated by file_descriptor, and writes into packet
 * 
 * @param file_descriptor Indicates the path to the Serial Port
 * @param packet The char pointer where the packet read from the Serial Port will be written
 * 
 * @return The size of the packet read
 */
int readInformationMessage(int file_descriptor, unsigned char * packet);

/**
 * @brief Calculates the Parity
 * @details Calculates the Parity of array with length length
 * 
 * @param array Contains the info whose Parity will be calculated
 * @param length The length of the info whose Parity will be calculated
 * 
 * @return The Parity calculated
 */
unsigned char calculateParity (unsigned char* array, int length);

/**
 * @brief Switches the sequence number
 * @details Switches the sequence number used in the Packets for error control
 */
void switchSequenceNumber ();

/**
 * @brief Stuffs a given array
 * @details Stuffs a given array with a given length and returns it in the form of a stuffed_message
 * 
 * @param array Contains the message to stuff
 * @param stuffed_message Points to the location where the stuffed message will be returned
 * @param length The length of the stuffed message
 * @return 0 on success, negative otherwise
 */
int byteStuffing(unsigned char* array, unsigned char** stuffed_message, int length);

/**
 * @brief Destuffs a given array
 * @details Stuffs a given array with a given length and returns it in the form of a destuffed_message
 * 
 * @param array Contains the message to destuff
 * @param stuffed_message Points to the location where the destuffed message will be returned
 * @param length The length of the destuffed message
 * @return 0 on success, negative otherwise
 */
int byteDeStuffing(unsigned char * array,unsigned char **destufed_message, int length);
