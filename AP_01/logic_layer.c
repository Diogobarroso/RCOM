#include "serial.h"
#include "application.h"

llOpen(struct applicationLayer * appLayer, struct linkLayer * lLayer)
{
	char * header = (char*) malloc (5*sizeof(unsigned char));

	openSerial(lLayer->port, lLayer->oldtio);	
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
		char c = readSerial(appLayer->fileDescriptor);
		printf("%c\n", c);
	}

}