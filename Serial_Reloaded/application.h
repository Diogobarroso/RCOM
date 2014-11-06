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

int client(char * port);

int server(char * port, char * file_name);

void alarmHandler(int signo);

int readPackage(int file_descriptor, unsigned char** data, int* bytesRead);

int readDataPackage(unsigned char* package, unsigned char** data, int* bytesRead);

int writeDataPackage(int file_descriptor, int sequenceNumber, int dataSize, unsigned char * data);

int readControlPackage(unsigned char * package, int size);
