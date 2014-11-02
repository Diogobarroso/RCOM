#pragma once

#define CLIENT 0
#define SERVER 1

#define C_INFO 1
#define C_START 2
#define C_END 3

#define START_2 2
#define START_3 3

#define FILE_SIZE 1
#define FILE_NAME 2

#define PACKET_SIZE 200

int main(int argc, char** argv);

/**
 * @brief Sends a Control Packet through the Serial Port
 * @details Writes a Control Packet with the following format:
 * C T L data
 * where C can be C_START(2) or C_END(3), t indicates what information is being sent: FILE_SIZE(1) or FILE_NAME(2), l is the length of data in bytes, data is the information to send
 * 
 * @param fd File Descriptor for the Serial Port
 * @param c Control Field
 * @param t Parameter
 * @param l Length (in bytes)
 * @param data Bytes to send
 * @return 0 on success, -1 otherwise
 */

int writeControlPacket(int fd, int c, int t, int l,unsigned  char* data);


/**
 * @brief Receives a Control Packet through the Serial Port
 * @details Reads a Control Packet with the following format:
 * C T L data
 * where C can be C_START(2) or C_END(3), t indicates what information is being sent: FILE_SIZE(1) or FILE_NAME(2), l is the length of data in bytes, data is the information to send
 * 
 * @param fd File Descriptor for the Serial Port
 * @param c Control Field
 * @param t Parameter
 * @param l Length (in bytes)
 * @param data Bytes to receive
 * @return 0 on success, -1 otherwise
 */

int readControlPacket(unsigned char* buffer, unsigned char* data, int b_length);

/**
 * @brief Sends an Information Packet through the Serial Port
 * @details Writes an Information Packet with the following structure:
 * C N L2 L1 P1 ... Pn
 * where:
 * C-> Control Field = 1
 * N-> Sequence Number
 * L2-> (length/256)
 * L1-> (length%256)
 * P1 ... Pn-> Information to write
 * It then calls llwrite with this array
 * 
 * @param fd File Descriptor of the Serial Port
 * @param lenth Length of data in bytes
 * @param data Char array to transmit
 * @return 0 on success, -1 otherwise
 */
int writeInfoPacket(int fd, int length, unsigned char* data);

/**
 * @brief Reads an Information Packet from the Serial Port
 * @details Reads the Information Packet from the Serial Port, in order to do this:
 * Reads the first byte, C, if it is 1, the Packet contains Information 
 * 
 * @param fd [description]
 * @param data [description]
 * @return [description]
 */

int readInfoPacket(unsigned char * buffer, unsigned char* data);


/**
 * @brief Reads a Packet
 * @details Reads a packet with llread, based on the C field, it will either call 
 * readControlPacket(fd, C_END, FILE_SIZE, 20, data)
 * readInfoPacket(fd, data)
 * 
 * @param fd File Descriptor of the Serial Port
 * @param data Pre-allocated array to return the packet
 * 
 * @return 0 on success, -1 otherwise
 */
int readPacket(int fd, unsigned char* data);
