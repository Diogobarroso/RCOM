#include "link_layer.h"

int sequenceNumber;
int nTriesAllowed;
int alarmTime;
int llOpen(unsigned char* port, int mode)
{
	sequenceNumber = 0;
	nTriesAllowed = 3;
	alarmTime = 3;

	int file_descriptor = openSerial(port);
	
	global_mode = mode;
	if (mode == SERVER)
	{
		int nTries = 0;
		while (nTries < nTriesAllowed)
		{
			alarm(alarmTime);

			writeSupervisionMessage(file_descriptor, SET);

			char controlField;
			int readResult = readSupervisionMessage(file_descriptor, &controlField);

			if (readResult == 0 && controlField == UA)
			{
				return file_descriptor;
			} else if (readResult < 0)
			{
				nTries ++;
			}
		}

		return -1;
	}

	else if (mode == CLIENT)
	{
		char controlField;
		while(1)
		{
			int readResult = readSupervisionMessage(file_descriptor, &controlField);

			if (readResult == 0 && controlField == SET)
			{
				writeSupervisionMessage(file_descriptor, UA);
				return file_descriptor;
			}
		}

		return -1;
	}

	return -1;
}



int llClose(int file_descriptor)
{
	if(global_mode == SERVER)
	{
		//Send a DISC byte to the client
		writeSupervisionMessage(file_descriptor, DISC);
		alarm(alarmTime);
		char controlField;
		int readResult = readSupervisionMessage(file_descriptor, &controlField);

		if (readResult == 0 && controlField == DISC)
		{
			writeSupervisionMessage(file_descriptor, UA);
			return file_descriptor;
		}
		
	} else if(global_mode == CLIENT) 
	{
		char controlField;
		int readResult = readSupervisionMessage(file_descriptor, &controlField);

		if (readResult == 0 && controlField == DISC)
		{
			writeSupervisionMessage(file_descriptor, DISC);
			return file_descriptor;
		}
	}

	printf("llClose\n");

	closeSerial(file_descriptor);
}

int llWrite(int file_descriptor, unsigned char* packet, int packetSize)
{
	int nTries = 0;
	//alarm(alarmTime);
	while (nTries < nTriesAllowed)
	{
		alarm(alarmTime);
		printf("before alarm!\n");

		
		//printf("before writeReturn!\n");
		int writeReturn = writeInformationMessage(file_descriptor, packet, packetSize);
		printf("After writeReturn! It returns %d\n", writeReturn);

		unsigned char controlField;
		int readResult = readSupervisionMessage(file_descriptor, &controlField);


		if (readResult == 0 && ( (controlField == RR0 && sequenceNumber == 0) || (controlField == RR1 && sequenceNumber == 1) ))
		{
			return writeReturn;
		}
		else
		{
			nTries++;
			printf("Increased nTries to %d\n",nTries);
		}
	}
	return(-1);
}

int llRead(int file_descriptor, unsigned char* packet)
{
	char controlField;

	while (1)
	{
		int readResult = readInformationMessage(file_descriptor, packet);


		if (readResult > 0)
		{
			if (sequenceNumber == 0)
				writeSupervisionMessage(file_descriptor, RR0);
			else if (sequenceNumber == 1)
				writeSupervisionMessage(file_descriptor, RR1);
			return readResult;

		} else if (readResult < 0)
		{
			printf("rej: sequence number = %d\n", sequenceNumber);
			if (sequenceNumber == 0)
			{
				printf("sent rej0\n");
				writeSupervisionMessage(file_descriptor, REJ0);
			}
			else if (sequenceNumber == 1)
			{
				printf("sent rej1\n");
				writeSupervisionMessage(file_descriptor, REJ1);
			}
			return -1;
		}
	}

	return(-1);
}

// Private

int writeSupervisionMessage(int file_descriptor, unsigned char controlField)
{
	char message[5];

	message[0] = FLAG;
	message[1] = ADDRESS;
	message[2] = controlField;
	message[3] = ADDRESS ^ controlField;
	message[4] = FLAG;


	int writeResult = writeSerial(file_descriptor, message, 5);

	return	writeResult;
}

int readSupervisionMessage(int file_descriptor, unsigned char * controlField)
{

	enum states state = START;

	unsigned char c, cField;
	int change = 0;
	int rej = 0;

	while (1)
	{
		int readResult = readSerial(file_descriptor, &c);

		if (readResult < 0)
		{
			return(-1);
		} else {
			alarm(0);
		}

		switch (state)
		{
			case (START):
			{
				switch (c)
				{
					case (FLAG):
					change = 0;
					state = FLAG_RCV;
					break;
				}
				break;
			}

			case (FLAG_RCV):
			{
				switch (c)
				{
					case (FLAG):
					state = FLAG_RCV;
					break;

					case (ADDRESS):
					state = A_RCV;
					break;

					default:
					state = START;
					break;
				}
				break;
			}

			case (A_RCV):
			{
				switch (c)
				{
					case (FLAG):
					state = FLAG_RCV;
					break;

					case (SET):
					case (DISC):
					case (UA):
					case (RR0):
					case (RR1):
					case (REJ0):
					case (REJ1):

					if ((c == RR0 && sequenceNumber == 1) || (c == RR1 && sequenceNumber == 0))
					{
						change = 1;
						state = C_RCV;
						cField = c;
					} else if (c == REJ0 || c == REJ1)
					{
						rej = 1;
						state = C_RCV;
						cField = c;
					} else if (c == SET || c == DISC || c == UA)
					{
						state = C_RCV;
						cField = c;
					}
					break;

					default:
					state = START;
					break;
				}
				break;
			}

			case (C_RCV):
			{
				if (cField == SET)
				{
					switch (c)
					{
						case (FLAG):
						state = FLAG_RCV;
						break;

						case (SET ^ADDRESS):
						state = BCC_OK;
						break;

						default:
						state = START;
						break;
					}
					break;
				} else if (cField == DISC)
				{
					switch (c)
					{
						case (FLAG):
						state = FLAG_RCV;
						break;

						case (DISC ^ ADDRESS):
						state = BCC_OK;
						break;

						default:
						state = START;
						break;
					}
				} else if (cField == UA)
				{
					switch (c)
					{
						case (FLAG):
						state = FLAG_RCV;
						break;

						case (UA ^ ADDRESS):
						state = BCC_OK;
						break;

						default:
						state = START;
						break;
					}
				} else if (cField == RR0)
				{
					switch (c)
					{
						case (FLAG):
						state = FLAG_RCV;
						break;

						case (RR0 ^ ADDRESS):
						state = BCC_OK;
						break;

						default:
						state = START;
						break;
					}
				} else if (cField == RR1)
				{
					switch (c)
					{
						case (FLAG):
						state = FLAG_RCV;
						break;

						case (RR1 ^ ADDRESS):
						state = BCC_OK;
						break;

						default:
						state = START;
						break;
					}
				} else if (cField == REJ0)
				{
					switch (c)
					{
						case (FLAG):
						state = FLAG_RCV;
						break;

						case (REJ0 ^ ADDRESS):
						state = BCC_OK;
						break;

						default:
						state = START;
						break;
					}
				} else if (cField == REJ1)
				{
					switch (c)
					{
						case (FLAG):
						state = FLAG_RCV;
						break;

						case (REJ1 ^ ADDRESS):
						state = BCC_OK;
						break;

						default:
						state = START;
						break;
					}
				}
				break;
			}

			case (BCC_OK):
			{
				switch (c)
				{
					case (FLAG):
					{
						* controlField = cField;
						if (rej == 1)
							return (-1);

						if (change == 1)
							switchSequenceNumber();
						return 0;
					}
					break;

					default:
					state = START;
					break;
				}
				break;

			}
		}
	}
}

int writeInformationMessage(int file_descriptor, unsigned char * packet, int packetSize)
{
	unsigned char * informationMessage = (unsigned char *) malloc ((IMH_SIZE + packetSize + IMT_SIZE) * sizeof(unsigned char));

	informationMessage[0] = FLAG;
	informationMessage[1] = ADDRESS;

	if (sequenceNumber == 0)
		informationMessage[2] = C0;
	else if (sequenceNumber == 1)
		informationMessage[2] = C1;

	informationMessage[3] = informationMessage[1] ^ informationMessage[2];

	memcpy(informationMessage + IMH_SIZE, packet, packetSize);

	informationMessage[IMH_SIZE + packetSize] = calculateParity(packet, packetSize) ;

	informationMessage[IMH_SIZE + packetSize + 1] = FLAG;

	unsigned char * stuffedInformationMessage;
	int stuffedSize = byteStuffing(informationMessage, &stuffedInformationMessage, (IMH_SIZE + packetSize + IMT_SIZE));

	int writeReturn = writeSerial(file_descriptor, stuffedInformationMessage, stuffedSize);

	return writeReturn;
}

int readInformationMessage(int file_descriptor, unsigned char * packet)
{
	enum states state = START;

	unsigned char c;
	int index = 0;
	int change = 0;

	unsigned char * message = malloc (2000);
	int messageIndex = 0;


/**	int readResult = readSerial(file_descriptor, &c);

	if (readResult < 0)
	{
		return(-1);
	} else {
		alarm(0);
	}

	message[messageIndex++] = c;

	do {
		int readResult = readSerial(file_descriptor, &c);

		if (readResult < 0)
		{
			return(-1);
		} else {
			alarm(0);
		}

		message[messageIndex++] = c;

	} while (c != FLAG);*/


	int i = 0;
	while (1)
	{
		int readResult = readSerial(file_descriptor, &c);
		if(readResult < 0)
			return -1;
		
		//c = message[i++];

		switch (state)
		{
			case (START):
			{
				switch (c)
				{
					case (FLAG):
					state = FLAG_RCV;
					memset(message, 0, sizeof(message));
					index = 0;
					break;
				}
				break;
			}

			case (FLAG_RCV):
			{
				switch (c)
				{
					case (FLAG):
					state = FLAG_RCV;
					break;

					case (ADDRESS):
					state = A_RCV;
					break;

					default:
					state = START;
					break;
				}
				break;
			}

			case (A_RCV):
			{
				switch (c)
				{
					case (FLAG):
					state = FLAG_RCV;
					break;

					case (C0):
					if (sequenceNumber == 0)
					{
						change = 1;
						state = C_RCV;
					}
					break;

					case (C1):
					if (sequenceNumber == 1)
					{
						change = 1;
						state = C_RCV;
					}
					break;

					default:
					state = START;
					break;
				}
				break;
			}

			case (C_RCV):
			{
				switch (c)
				{
					case (FLAG):
					state = FLAG_RCV;
					break;

					case (C0 ^ ADDRESS):
					if (sequenceNumber == 0)
						state = BCC_OK;
					break;

					case (C1 ^ ADDRESS):
					if (sequenceNumber == 1)
						state = BCC_OK;
					break;

					default:
					state = START;
					break;

				}
				break;
			}

			case (BCC_OK):
			{
				switch (c)
				{
					case (FLAG):
					{

						unsigned char * destufed_message;
						int length = byteDeStuffing(message, &destufed_message, index);

						unsigned char xr = calculateParity(destufed_message, length - 1);

						if (xr != destufed_message[length - 1])
						{
							printf("Parity check failed\nXR = %X\n",xr);
							return -1;
						}

						if (change == 1)
							switchSequenceNumber();

						memcpy(packet,destufed_message,length - 1);

						return (length - 1);
					}
					break;

					default:
					message[index++] = c;
					break;
				}
				break;

			}
		}
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

	printf("Changed Sequence Number to %d\n", sequenceNumber);
}

int byteStuffing(unsigned char* array, unsigned char** stuffed_message, int length)
{

	int special_chars = 0;
	int i = 1;

	for(i; i < length - 1; i++)
	{
		if(array[i] == ESCAPE || array[i] == FLAG)
			special_chars++;
	}

	int stuffed_size = length + special_chars;

	*stuffed_message = (unsigned char *) malloc(stuffed_size * sizeof(unsigned char));

	(*stuffed_message)[0] = array[0];

	i = 1;
	int stuffed_message_current_index = 1;

	for(i; i < length - 1; i++)
	{
		if(array[i] == ESCAPE)
		{
			//printf("ESCAPE found\n");
			(*stuffed_message)[stuffed_message_current_index++] = ESCAPE;
			(*stuffed_message)[stuffed_message_current_index++] = ESCAPE ^ STUFCHAR;
		} else if(array[i] == FLAG)
		{
			//printf("FLAG found\n");
			(*stuffed_message)[stuffed_message_current_index++] = ESCAPE;
			(*stuffed_message)[stuffed_message_current_index++] = FLAG ^ STUFCHAR;
		} else
		{
			(*stuffed_message)[stuffed_message_current_index++] = array[i];
		}
	}

	(*stuffed_message)[stuffed_message_current_index] = array[i];

	return (sizeof(char) * stuffed_size);
}

int byteDeStuffing(unsigned char * array, unsigned char **destufed_message, int length)
{
	*destufed_message = (char *) malloc(sizeof(char) * length);
	int i = 0;
	int current_destuff_index = 0;

	for(i; i < length; i++)
	{
		if(array[i] == ESCAPE)
		{
			(*destufed_message)[current_destuff_index++] = array[++i] ^ STUFCHAR;
		}
		else
			(*destufed_message)[current_destuff_index++] = array[i];
	}
/*
	printArray(array, length, "Original");
	printArray(*destufed_message, current_destuff_index, "DeStuffed");
*/
	return sizeof(char) * current_destuff_index;
}
