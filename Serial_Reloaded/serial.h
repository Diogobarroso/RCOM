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
/**
 * @brief Opens the Serial
 * @details Opens the Serial
 * 
 * @param path The path to the Serial Port that will be used to communicate with the other computer
 * @return 0 on sucess, negative otherwise
 */
int openSerial (char* path);

/**
 * @brief Closes the Serial
 * @details Closes the Serial
 */
void closeSerial ();

/**
 * @brief Writes to the Serial Port
 * @details Writes the first bufferSize chars of buffer[] to the Serial Port indicated by file_descriptor
 * 
 * @param file_descriptor The file descriptor of the Serial Port
 * @param buffer[] An array with what will be written to the Serial Port
 * @param bufferSize The size of what will be written to the Serial Port
 * @return 0 on success, negative otherwise
 */
int writeSerial (int file_descriptor, unsigned char buffer[], int bufferSize);

/**
 * @brief Reads from the Serial Port
 * @details Reads from the Serial Port indicated by file_descriptor into c
 * 
 * @param file_descriptor The file descriptor of the Serial Port
 * @param c The pointer to where the function will write what is read from the Serial Port
 * 
 * @return the size of the array that was read from the Serial Port
 */
int readSerial (int file_descriptor, unsigned char* c);

// Configuration

/**
 * @brief Sets a new BaudRate
 * @details Sets the BaudRate to baudRate
 * 
 * @param baudRate The baudRate the application will use
 */
void setBaudRate (int baudRate);