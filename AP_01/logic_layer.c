#include "serial.h"
#include "logic_layer.h"
#include "application.h"

int sequenceNumber;

int llOpen(char * port, int mode)
{
	printf("llOpen (%s, %d)\n", port, mode);
	sequenceNumber = 0;

	char * header = (char*) malloc (5*sizeof(unsigned char));

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

			char c;
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
		char c;
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

int llwrite(int fd, char * buffer, int length)
{
	
	char * sequence = (char *) malloc ((6 + length) * sizeof (char));

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

	printf("Sequence:\n\tF->%X\n\tA->%X\n\tC->%X\n\tBCC->%X\n", sequence[0], sequence[1], sequence[2], sequence[3]);
	int index = 4;
	for (index = 4; index < length + 4; index++)
	{
		printf("\tbuffer[%d]->%c\n",index -4, sequence[index]);
	}
	printf("\tBCC->%X\n\tF->%X\n", sequence[length + 5], sequence[length + 6]);


	int writeReturn = writeSerial(sequence, (7 + length), fd);


	if (writeReturn != (7 + length))
	{
		printf("Error writing to the Serial Port\n");
		return (-1);
	} else {
		return writeReturn;
	}
}

int llread(int fd, char * buffer)
{

	enum states state = START;
	int parsing = 1;
	char c;
	int index = 0;

	while (parsing == 1)
	{
		int r = readSerial(fd, &c);

		if (r < 0)
		{
			printf("Error reading from Serial Port\n");
		}

		printf("\tc = %X | %c\n",c,c);

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
					printf("Unexpected sequence number\n");
				break;

				case (C1):
				if (sequenceNumber == 1)
					state = C_RCV;
				else
					printf("Unexpected sequence number\n");
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
					printf("Unexpected sequence number\n");
				break;

				case (A^C1):
				if (sequenceNumber == 1)
					state = BCC_OK;
				else
					printf("Unexpected sequence number\n");
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

	char xr = calculateParity(buffer, index - 2);

	if (xr == buffer[index - 1])
	{
		printf("Success reading Packet\n");
	}
}

char calculateParity (char* array, int length)
{
	char xr;

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

void switchSequenceNumber ()
{
	if (sequenceNumber == 1)
		sequenceNumber = 0;
	else
		sequenceNumber = 1;
}