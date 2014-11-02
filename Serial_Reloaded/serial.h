#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Public

int openSerial (char* path);

void closeSerial ();

int writeSerial (int file_descriptor, unsigned char buffer[], int bufferSize);

int readSerial (int file_descriptor, unsigned char* c);

// Configuration

void setBaudRate (int baudRate);