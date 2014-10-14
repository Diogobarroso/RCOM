#include "serial.c"

#define SET 0x03
#define UA 0x07

int main(int argc, char** argv)
{
	struct termios * oldtio;
	int file_descriptor = openSerial(argv[1], oldtio);

	printf("Server\n");

	closeSerial(file_descriptor, oldtio);
}