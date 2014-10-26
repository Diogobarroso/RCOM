#pragma once

#define CLIENT 0
#define SERVER 1

#define START_2 2
#define START_3 3

#define FILE_SIZE 1
#define FILE_NAME 2



#define PACKET_SIZE 200

/**
 * @brief Structure containing Serial Connection properties
 * @details Holds the file descriptor and status of the program
 * 
 */
struct applicationLayer {

	int fileDescriptor;	/** File descriptor for the Serial Port */

	int status; /** Client (0) / Server (1) */
};

int main(int argc, char** argv);

/**
 * @brief Sends a Control Packet through the Serial Port
 * @details Writes a Control Packet with the following format:
 * C T L data
 * where C can be START(2) or END(3), t indicates what information is being sent: FILE_SIZE(1) or FILE_NAME(2), l is the length of data in bytes, data is the information to send
 * 
 * @param fd File Descriptor for the Serial Port
 * @param c Control Field
 * @param t Parameter
 * @param l Length (in bytes)
 * @param data Bytes to send
 * @return 0 on success, -1 otherwise
 */
int writeControlPacket(int fd, int c, int t, int l, char* data);

/**
 * @brief Receives a Control Packet through the Serial Port
 * @details Reads a Control Packet with the following format:
 * C T L data
 * where C can be START(2) or END(3), t indicates what information is being sent: FILE_SIZE(1) or FILE_NAME(2), l is the length of data in bytes, data is the information to send
 * 
 * @param fd File Descriptor for the Serial Port
 * @param c Control Field
 * @param t Parameter
 * @param l Length (in bytes)
 * @param data Bytes to receive
 * @return 0 on success, -1 otherwise
 */
int readControlPacket(int fd, int c, int t, int l, char* data);
