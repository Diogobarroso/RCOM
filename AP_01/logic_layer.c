#include "serial.h"
#include "logic_layer.h"
#include "application.h"

llOpen(struct applicationLayer * appLayer, struct linkLayer * lLayer)
{
	char * header = (char*) malloc (5*sizeof(unsigned char));

	appLayer->fileDescriptor = openSerial(lLayer->port, lLayer->oldtio);

	enum states state = START;

	if (appLayer->status == SERVER)
	{
		header[0] = F;
		header[1] = A;
		header[2] = SET;
		header[3] = header[1] ^header[2];
		header[4] = F;
		writeSerial(header, 5, appLayer->fileDescriptor);
	}

	if (appLayer->status == CLIENT)
	{
		char c;
		int parsing = 1;
		/* State Machine for SET processing */
		while (parsing == 1)
		{
			c = readSerial(appLayer->fileDescriptor);
			printf("\n\n");

			switch (state)
			{
				case (START):
				printf("START\n");
				switch (c)
				{
					case (F):
					state = FLAG_RCV;
					break;
				}
				break;

				case (FLAG_RCV):
				printf("FLAG_RCV\n");
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
				printf("A_RCV\n");
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
				printf("C_RCV\n");
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
				printf("BCC_OK\n");
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
			}
		printf("%X\n", c);
		}

		printf("\nSUCCESS\n");
	}

}