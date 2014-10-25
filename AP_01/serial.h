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

/**
 * @brief Structure containing Serial Connection properties
 * @details Holds the port name, baudRate, sequenceNumber of the packet being transmitted, timeout value, number of retries for the transmission and the previous Serial Port configurations
 * 
 */
struct linkLayer {
	char port[20];	/** Port name */

	int baudRate; /** Transmission speed */

	unsigned int sequenceNumber; /** Sequence number: 0, 1 */

	unsigned int timeout; /** Timeout value */

	unsigned int numTransmissions; /** Number of retries for the transmission */

	char frame[5]; /** Frame */
	
	struct termios * oldtio; /** Previous Serial Port configurations */
};

/**
 * @brief Opens Serial Port
 * @details Starts connection through serial port
 * 
 * @param path Path to the Serial port
 * @param termios Structure to hold the old configuration of the serial port
 * 
 * @return 0 on success, -1 otherwise
 */
int openSerial (char* path, struct termios * oldtio);

/**
 * @brief Closes Serial Port
 * @details Closes the connection through the serial port
 * 
 * @param file_descriptor File descriptor for the serial port
 * @param termios Structure holding the old configuration of the serial port
 */
void closeSerial (int file_descriptor, struct termios * oldtio);

/**
 * @brief Writes to the Serial Port
 * @details Writes a message to the Serial Port identified in the file_descriptor
 * 
 * @param message Character array to be sent
 * @param messageSize Size of the message in bytes
 * @param file_descriptor File Descriptor for the Serial Port
 * @return Number of bytes written
 */
int writeSerial (char message[], int messageSize, int file_descriptor);

/**
 * @brief Reads from the Serial Port
 * @details Reads a character from the Serial Port
 * 
 * @param file_descriptor File Descriptor for the Serial Port
 * @param c Pointer to character to be read
 * 
 * @return 1 on success, -1 otherwise
 */
int readSerial (int file_descriptor, char * c);