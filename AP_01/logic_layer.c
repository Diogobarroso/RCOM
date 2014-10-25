#include "serial.h"
#include "logic_layer.h"
#include "application.h"

int llOpen(struct applicationLayer * appLayer, struct linkLayer * lLayer)
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

		int tries = 0;

		int parsing = 1;

		while (tries < 3)
		{
			if (parsing == 0)
			{
				break;
			}

			alarm(3);

			writeSerial(header, 5, appLayer->fileDescriptor);

			char c;
			/* State Machine for UA processing */
			while (parsing == 1)
			{
				int r = readSerial(appLayer->fileDescriptor, &c);

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

	if (appLayer->status == CLIENT)
	{
		char c;
		int parsing = 1;
		int success = 0;
		/* State Machine for SET processing */
		while (parsing == 1)
		{
			int r = readSerial(appLayer->fileDescriptor, &c);

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

			writeSerial(header, 5, appLayer->fileDescriptor);
		}
	}

	return appLayer->fileDescriptor;

}

int llwrite(int fd, char * buffer, int length)
{

}

