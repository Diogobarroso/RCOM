#pragma once

#define CLIENT 0
#define SERVER 1

#define PACKET_SIZE 200

struct applicationLayer {
	/*Descritor correspondente à porta série*/
	int fileDescriptor;	
	/* CLIENT / SERVER */
	int status; 	
};

int main(int argc, char** argv);

int readControlPacket(int fd, char* controlPacket);
