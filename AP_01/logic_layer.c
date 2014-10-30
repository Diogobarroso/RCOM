#include "serial.h"
#include "logic_layer.h"
#include "application.h"

int sequenceNumber;

int llOpen(unsigned char * port, int mode)
{
	printf("llOpen (%s, %d)\n", port, mode);
	sequenceNumber = 0;

	unsigned char * header = (unsigned char*) malloc (5*sizeof(unsigned char));

	int fd = openSerial(port);

	if (fd < 0)
		printf("Error opening serial port\n");

	enum states state = START;

	if (mode == SERVER)
	{
		header[0] = F;
		header[1] = A;
		header[2] = SET;
		header[3] = header[1] ^header[2];
		header[4] = F;

		int tries = 0;

		int parsing = 1;

		while (tries < 3)
		{
			if (parsing == 0)
			{
				break;
			}

			alarm(3);

			writeSerial(header, 5, fd);

			unsigned char c;
			/* State Machine for UA processing */
			while (parsing == 1)
			{
				int r = readSerial(fd, &c);

				printf("\tParsing character %X\n", c);

				if (r < 0)
				{
					tries++;
					break;
				} else {
					alarm(0);
				}

				switch (state)
				{
					case (START):
					switch (c)
					{
						case (F):
						state = FLAG_RCV;
						break;

					}
					break;

					case (FLAG_RCV):
					switch (c)
					{
						case (F):
						state = FLAG_RCV;
						break;

						case (A):
						state = A_RCV;
						break;

						default:
						state = START;
						break;
					}
					break;

					case (A_RCV):
					switch (c)
					{
						case (F):
						state = FLAG_RCV;
						break;

						case (UA):
						state = C_RCV;
						break;

						default:
						state = START;
						break;
					}
					break;

					case (C_RCV):
					switch (c)
					{
						case (F):
						state = FLAG_RCV;
						break;

						case (A^UA):
						state = BCC_OK;
						break;

						default:
						state = START;
						break;
					}
					break;

					case (BCC_OK):
					switch (c)
					{
						case (F):
						parsing = 0;
						break;

						default:
						state = START;
						break;
					}
					break;

					default:
					break;
				}

			}
		}
	}

	if (mode == CLIENT)
	{
		unsigned char c;
		int parsing = 1;
		int success = 0;
		/* State Machine for SET processing */
		while (parsing == 1)
		{
			int r = readSerial(fd, &c);

			printf("\tParsing character %X\n", c);

			switch (state)
			{
				case (START):
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					break;
				}
				break;

				case (FLAG_RCV):
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					break;

					case (A):
					state = A_RCV;
					break;

					default:
					state = START;
					break;
				}
				break;

				case (A_RCV):
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					break;

					case (SET):
					state = C_RCV;
					break;

					default:
					state = START;
					break;
				}
				break;

				case (C_RCV):
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					break;

					case (A^SET):
					state = BCC_OK;
					break;

					default:
					state = START;
					break;
				}
				break;

				case (BCC_OK):
				switch (c)
				{
					case (F):
					parsing = 0;
					success = 1;
					break;

					default:
					state = START;
					break;
				}
				break;
			}
		}

		if (success)
		{
			header[0] = F;
			header[1] = A;
			header[2] = UA;
			header[3] = header[1] ^header[2];
			header[4] = F;

			writeSerial(header, 5, fd);
		}
	}

	return fd;

}

int llwrite(int fd, unsigned char * buffer, int length)
{
	
	unsigned char * sequence = (unsigned char *) malloc ((6 + length) * sizeof (unsigned char));

	sequence[0] = F;
	sequence[1] = A;
	if (sequenceNumber == 0)
		sequence[2] = C0;
	else
		sequence[2] = C1;
	sequence[3] = sequence[1] ^ sequence[2];
	memcpy(sequence + 4, buffer, length);
	sequence[4 + length + 1] = calculateParity(& sequence[4],length);
	sequence[4 + length + 2] = F;
/*
	printf("Sequence:\n\tF->%X\n\tA->%X\n\tC->%X\n\tBCC->%X\n", sequence[0], sequence[1], sequence[2], sequence[3]);
	int index = 4;
	for (index = 4; index < length + 4; index++)
	{
		printf("\tbuffer[%d]->%X\n",index -4, sequence[index]);
	}
	printf("\tBCC->%X\n\tF->%X\n", sequence[length + 5], sequence[length + 6]);
*/

	char *stuffed_message;
	int bytes = byteStuffing(sequence, stuffed_message, 6 + length);
	int writeReturn = writeSerial(stuffed_message, bytes, fd);



	if (writeReturn != (7 + length))
	{
		printf("Error writing to the Serial Port\n");
		return (-1);
	} else {
		return writeReturn;
	}
}

int llread(int fd, unsigned char * buffer)
{
	enum states state = START;
	int parsing = 1;
	unsigned char c;
	int index = 0;
	
	while (parsing == 1)
	{
		int r = readSerial(fd, &c);

		if (r < 0)
		{
			printf("Error reading from Serial Port\n");
		}

		printf("\tc = %X | %c | %d\n",c,c,c);

		switch (state)
		{
			case (START):
			switch (c)
			{
				case (F):
				state = FLAG_RCV;
				break;

				default:
				state = START;
				break;
			}
			break;

			case (FLAG_RCV):
			switch (c)
			{
				case (F):
				state = FLAG_RCV;
				break;

				case (A):
				state = A_RCV;
				break;

				default:
				state = START;
				break;
			}
			break;

			case (A_RCV):
			switch (c)
			{
				case (F):
				state = FLAG_RCV;
				break;

				case (C0):
				if (sequenceNumber == 0)
					state = C_RCV;
				else
				{
					printf("Unexpected sequence number\n");
					printf("Sequence number is %u\n", sequenceNumber);
				}
				break;

				case (C1):
				if (sequenceNumber == 1)
					state = C_RCV;
				else
				{
					printf("Unexpected sequence number\n");
					printf("Sequence number is %u\n", sequenceNumber);
				}
				break;

				default:
				state = START;
				break;
			}
			break;

			case (C_RCV):
			switch (c)
			{
				case (F):
				state = FLAG_RCV;
				break;

				case (A^C0):
				if (sequenceNumber == 0)
					state = BCC_OK;
				else
				{
					printf("Unexpected sequence number\n");
					printf("Sequence number is %u\n", sequenceNumber);
				}
				break;

				case (A^C1):
				if (sequenceNumber == 1)
					state = BCC_OK;
				else
				{
					printf("Unexpected sequence number\n");
					printf("Sequence number is %u\n", sequenceNumber);
				}
				break;

				default:
				state = START;
				break;
			}
			break;

			case (BCC_OK):
			switch (c)
			{
				case(F):
				parsing = 0;
				break;

				default:
				buffer[index++] = c;
				break;
			}
			break;
		}
	}

	printf("Done parsing\n");

	char *destuffed_message;
	byteDeStuffing(buffer, destuffed_message, index);
	unsigned char xr = calculateParity(destuffed_message, index);

	if (xr == buffer[index - 1])
	{
		printf("Success reading Packet\n");
	}

}

unsigned char calculateParity (unsigned char* array, int length)
{
	unsigned char xr;

	if (length < 2) {
		xr = array[0];
    } else if (length == 2) {
    	xr = array[0] ^ array[1];
    } else if (length > 2) {
    	xr = array[0] ^ array[1];
    	int i;
    	for (i = 2; i < length; i ++) {
    		xr = xr ^ array[i];
    	}
    }

    return xr;
}

void switchSequenceNumber()
{
	if (sequenceNumber == 1)
		sequenceNumber = 0;
	else
		sequenceNumber = 1;
}


int sendSuperPacket(int fd, unsigned char c)
{
	unsigned char* header = (unsigned char *) malloc (5*sizeof(unsigned char));
	
	printf("header[0] = %X\n",header[0]);
	printf("header[2] = %X\n",header[2]);
	header[0] = F;
	header[1] = A;
	if (c == RR)
	{
		if (sequenceNumber == 0)
		{	printf("RR0\n");
			header[2] = RR0;
		}
		else
		{	printf("RR1\n");
			header[2] = RR1;
		}
	} else if (c == REJ)
	{
		printf("REJ\n");
		if (sequenceNumber == 0)
		{	printf("REJ0\n");
			header[2] = REJ0;
		}
		else
		{	printf("REJ1\n");
			header[2] = REJ1;
		}
	} else 
	{
		header[2] = c;
	}
	
	printf("header[2] = %X\n",header[2]);
	
	header[3] = header[1] ^header[2];
	header[4] = F;
	
	int ret = llwrite(fd, header, 5);
	
	return ret;
}


int readSuperPacket(int fd)
{
	int parsing = 1, rr = 0;
		
	enum states state = START;

	alarm(3);
	unsigned char c;
	while(parsing == 1)
	{
		int r = readSerial(fd, &c);
		printf("Parsing character %X\n",c);
		if(r < 0)
			return -1;
		else
			alarm(0);

		switch (state)
		{
				case (START):
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					printf("FLAG_RCV\n");
					break;
				}
				break;
				case (FLAG_RCV):
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					printf("FLAG_RCV\n");
					break;
					case (A):
					state = A_RCV;
					printf("A_RCV\n");
					break;
					default:
					state = START;
					break;
				}
				break;
				case (A_RCV):
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					printf("FLAG_RCV\n");
					break;

						case (RR1):
							if (sequenceNumber == 0)
{
							state = C_RCV;
							printf("C_RCV\n");
							break;
}
						case(REJ1):
							if (sequenceNumber == 0)
{
							rr=1;
							break;
}
					
					else if(sequenceNumber == 1)
					{
						case(RR0):
							if (sequenceNumber == 1)
{
							state = C_RCV;
							printf("C_RCV\n");
							break;
}
						case(REJ0):
							if (sequenceNumber == 1)
{
							rr=1;
							break;
}
					
					}

					default:
					state = START;
					break;
				}
				break;
				case (C_RCV):
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					printf("FLAG_RCV\n");
					break;

					if(sequenceNumber == 0)
					{
						printf("Sequence = 0\n");
						case (A^RR1):
							state = BCC_OK;
							printf("BCC_OK\n");
							break;
						case(A^REJ1):
							rr = 1;
							break;
					}
					else if(sequenceNumber == 1)
					{
						printf("Sequence = 1\n");
						case(A^RR0):
							state = BCC_OK;
							printf("BCC_OK\n");
							break;
						case(A^REJ0):
							rr = 1;
							break;
					}
					default:
					state = START;
					break;
				}
				break;
				case (BCC_OK):
				switch (c)
				{
					case (F):
					parsing = 0;
					break;
					default:
					state = START;
					break;
				}
				break;

				default:
				break;
			}
		}
	return rr;
}



int byteStuffing(unsigned char* array, char* stuffed_message, int length)
{
	int special_chars = 0;
	int i = 1;
	
	for(i; i < length - 1; i++)
		if(array[i] == ESCAPE || array[i] == F)
			special_chars++;
			
	int stuffed_size = length + special_chars;
	stuffed_message = (char *) malloc(sizeof(char) * stuffed_size);
	
	stuffed_message[0] = array[0];
	i = 1;
	int stuffed_message_current_index = i;
	
	
	for(i; i < length - 1; i++)
	{
		if(array[i] == ESCAPE)
		{
			stuffed_message[stuffed_message_current_index++] = ESCAPE;
			stuffed_message[stuffed_message_current_index] = ESCAPE ^ STUFCHAR;
		} else if(array[i] == F) {
			stuffed_message[stuffed_message_current_index++] = ESCAPE;
			stuffed_message[stuffed_message_current_index] = F ^ STUFCHAR;
		} else 
			stuffed_message[stuffed_message_current_index++] = array[i];
	}
	
	stuffed_message[stuffed_message_current_index] = array[i];
	return sizeof(char) * stuffed_size;
}



int byteDeStuffing(unsigned char * array, char *destufed_message, int length)
{
	destufed_message = (char *) malloc(sizeof(char) * length);
	
	int i = 0;
	int current_destuff_index = 0;
	for(i; i < length; i++)
	{
		if(array[i] == ESCAPE)
			destufed_message[current_destuff_index++] = array[i + 1] ^ STUFCHAR;
		else
			destufed_message[current_destuff_index++] = array[i];
	}
	
	return sizeof(char) * length;
}
