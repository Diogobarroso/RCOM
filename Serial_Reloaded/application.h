#pragma once

#include "link_layer.h"

#include <stdio.h>
#include <stdlib.h>

// Macros

#define CLIENT 		0
#define SERVER 		1

#define C_DATA		0x01
#define C_START		0x02
#define C_END		0x03

#define T_NAME		0x10
#define T_SIZE		0x11

#define DPH_SIZE	4

#define MAX_PACKAGE_SIZE 2000

// Public

int main(int argc, char** argv);

// Private
/**
 * @brief The code to be executed by the Client (receiver)
 * @details The code to be executed by the Client (receiver)
 * 
 * @param port Indicates the path to the Serial Port
 * @return 0 on success, negative otherwise
 */
int client(char * port);

/**
 * @brief The code to be executed by the Server (emitter)
 * @details The code to be executed by the Server (emitter)
 * 
 * @param port Indicates the path to the Serial Port
 * @param file_name Indicates the path to the file_name to be sent
 * @return 0 on success, negative otherwise
 */
int server(char * port, char * file_name);

/**
 * @brief Handles the alarm
 * @details Handles the alarm that is used by the Server (emitter)
 * 
 * @param signo the Signal received by the alarmHandler
 */
void alarmHandler(int signo);

/**
 * @brief Reads a Package
 * @details Reads a Package and calls either readDataPackage or readControlPackage, depending on the header of the Package
 * 
 * @param file_descriptor Indicates the path to the Serial Port
 * @param data Where the Package will be written on
 * @param bytesRead The size of data
 * @return 0 on success, negative otherwise
 */
int readPackage(int file_descriptor, unsigned char** data, int* bytesRead);

/**
 * @brief Reads a Data Package
 * @details Reads a Data Package and writes its' content to data
 * 
 * @param package The untreated Package
 * @param data Where the Package will be written on
 * @param bytesRead The size of data
 * @return 0 on success, negative otherwise
 */
int readDataPackage(unsigned char* package, unsigned char** data, int* bytesRead);

/**
 * @brief Writes a Data Package
 * @details Writes a Data Package to file_descriptor
 * 
 * @param file_descriptor Indicates the path to the Serial Port
 * @param sequenceNumber The sequenceNumber of the package that will be written, for error control
 * @param dataSize The size of the Package that will be written
 * @param data Contains the data that will be written to the Serial Port
 * @return 0 on success, negative otherwise
 */
int writeDataPackage(int file_descriptor, int sequenceNumber, int dataSize, unsigned char * data);

/**
 * @brief Reads a Control Package
 * @details Reads a Data Package and interprets it
 * 
 * @param package The untreated Package
 * @param size The size of the untreated package
 * @return 0 on success, negative otherwise
 */
int readControlPackage(unsigned char * package, int size);
