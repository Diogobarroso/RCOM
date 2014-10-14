#pragma once

#define CLIENT 0
#define SERVER 1

struct applicationLayer {
	/*Descritor correspondente à porta série*/
	int fileDescriptor;	
	/* CLIENT / SERVER */
	int status; 	
};

int main(int argc, char** argv);
