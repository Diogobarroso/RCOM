#include "application.h"

int packageSize;
char * file_path;
int file_size;

int main(int argc, char** argv)
{
	/* Set allarm */
	struct sigaction act;
	act.sa_handler = alarmHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGALRM,&act,NULL) == -1)
	{
		printf("Error in sigaction\n");
		return(-1);
	}
	printf("Set alarm\n");

	int result = -1;
	printf("Setting client/server\n");
	if (strcmp("client", argv[2]) == 0)
	{
		int tmp = atoi(argv[3]);
		if(tmp == 0 || tmp > MAX_PACKAGE_SIZE)
		{
			printf("[!] Error! No valid conversion could be performed! packageSize is now 200");
			packageSize = 200;
		} else
			packageSize = tmp;
		result = client(argv[1]);
	}

	if (strcmp("server", argv[2]) == 0)
	{
		int tmp = atoi(argv[4]);
		if(tmp == 0 || tmp > MAX_PACKAGE_SIZE)
		{
			printf("[!] Error! No valid conversion could be performed! PackageSize is now 200\n");
			packageSize = 200;

		} else
			packageSize = tmp;

		printf("Package Size: %d\n", packageSize);

		result = server(argv[1], argv[3]);
	}

	return result;
}

int client(char * port)
{
	int file_descriptor = llOpen(port, CLIENT);

	if (file_descriptor < 0)
	{
		printf("Error opening Serial Port\n");
		return (-1);
	}

	int readReturn;
	int bytesRead = 0;
	file_path = (unsigned char *) malloc (sizeof(unsigned char) * 24);
	
	unsigned char* data;

	readReturn = readPackage(file_descriptor, &data, &bytesRead);
/**	if(readReturn < 0) {
		//llClose(file_descriptor);
		exit(1);
	}*/

	char * new_file_name;
	char * file_name_header = "copy_";
	if((new_file_name = malloc(strlen(file_path)+strlen(file_name_header)+1)) != NULL)
	{
		new_file_name[0] = '\0';  
		strcat(new_file_name,file_name_header);
		strcat(new_file_name,file_path);
		new_file_name[strlen(file_path)+strlen(file_name_header)] = '\0';
	}

	FILE * fo = fopen(new_file_name, "wb");
	if (fo == NULL)
	{
		printf("Error opening file %s\nThe program will now exit\n", new_file_name);
		return(-1);
	}

	while(1)
	{	
		readReturn = readPackage(file_descriptor, &data, &bytesRead);
		if(readReturn == C_END)
			break;
		else 
		{
			if(readReturn != -1) {
				//printArray(data, bytesRead, "Package");

				fwrite(data, bytesRead, 1, fo);
				//getchar();

				printf("\n\n\n");
			}
		}
	} //while (readReturn != C_END);
	llClose(file_descriptor);

	return 0;
}

int server(char * port, char* file_name)
{

	int file_descriptor = llOpen(port, SERVER);
	if (file_descriptor < 0)
	{
		printf("Error opening Serial Port\n");
		return (-1);
	}

	/* Read File */
	FILE* fr = fopen(file_name, "rb");
	if (fr == NULL)
	{
		printf("Error opening file %s\nThe program will now exit\n", file_name);
		return(-1);
	}

	/* Get File Size */
	fseek(fr, 0L, SEEK_END);
	file_size = ftell(fr);
	fseek(fr, 0L, SEEK_SET);

	char file_size_txt [9];
	sprintf(file_size_txt, "%d", file_size);

	unsigned char types [2];
	types[0] = T_NAME;
	types[1] = T_SIZE;

	unsigned char lengths [2];
	lengths[0] = strlen(file_name);
	lengths[1] = strlen(file_size_txt);

	char * values [2];
	values[0] = file_name;
	values[1] = file_size_txt;
	
	if( writeControlPackage(file_descriptor, C_START, 2, types, lengths, values) < 0)
		llClose(file_descriptor);

	unsigned char * buffer = malloc (sizeof(unsigned char * ) * packageSize);

	int bytesRead = 0, total = 0, sequence = 0;

	while (!feof (fr))
	{
		bytesRead = fread(buffer, 1, packageSize, fr);
		if (ferror (fr))
		{
			perror("Read Error");
			exit(1);
		}

		//printArray(buffer, bytesRead, "Package");
		total += bytesRead;

		if (writeDataPackage(file_descriptor, sequence, bytesRead, buffer) < 0)
		{
			llClose(file_descriptor);
			exit(1);
		}
	printf("\n\n\n");
	}

	writeControlPackage(file_descriptor, C_END, 2, types, lengths, values);

	llClose(file_descriptor);

	return 0;
}

int readPackage(int file_descriptor, unsigned char** data, int* bytesRead)
{
	unsigned char * package = (unsigned char *) malloc (DPH_SIZE + packageSize);
	int readReturn = llRead(file_descriptor, package);
	if(readReturn >= 0)
	{
		if (package[0] == C_DATA)
		{
			readDataPackage(package, data, bytesRead);
			return C_DATA;
		} else if (package[0] == C_START)
		{
			readControlPackage(package, readReturn);
			return C_START;
		} else if (package[0] == C_END)
		{
			readControlPackage(package, readReturn);
			return C_END;
		} else
		{
			return -1;
		}	
	} else 
		return -1;
}


int readDataPackage(unsigned char * package, unsigned char** data, int* bytesRead)
{
	int l2 = package[2];
	int l1 = package[3];

	int pSize = (255 * l2) + l1;

	*data = (unsigned char*) malloc (pSize * sizeof(unsigned char));

	memcpy((*data), package + 4, pSize);

	*bytesRead = pSize;
}

int readControlPackage(unsigned char * package, int size)
{
	int index = 1;

	while (index < size)
	{
		unsigned char type = package[index++];
		int length = package[index++];
		unsigned char * buffer = (unsigned char *) malloc (length);
		memcpy(buffer, package + index++, (int) length);
		index += length;

		if (type == T_NAME)
		{
			memcpy(file_path,buffer,length);
		} else if (type == T_SIZE)
		{
			file_size = buffer;
		}
	}
}

void alarmHandler(int signo)
{
	printf("Alarm Handler\n");
}

int writeDataPackage(int file_descriptor, int sequenceNumber, int dataSize, unsigned char * data)
{
	unsigned char * dataPackage = (unsigned char *) malloc ((DPH_SIZE + dataSize) * sizeof (unsigned char));

	dataPackage[0] = C_DATA;
	dataPackage[1] = (sequenceNumber % 255);
	dataPackage[2] = dataSize / 255;
	dataPackage[3] = dataSize % 255;
	memcpy(dataPackage + 4, data, dataSize);

	if(llWrite(file_descriptor, dataPackage, (DPH_SIZE + dataSize)) == -1)
		return -1;
	else
		return 0;
}

int writeControlPackage(int file_descriptor, int controlField, int itemCount, unsigned char * types, unsigned char * lengths, unsigned char * * values)
{
	int totalSize = (1 + (itemCount * 2));

	int index;
	for (index = 0; index < itemCount; index++)
	{
		totalSize += lengths[index];
	}
	
	unsigned char * controlPackage = (unsigned char *) malloc (totalSize * sizeof(unsigned char));

	controlPackage[0] = controlField;

	int packageIndex = 1;
	for (index = 0; index < itemCount; index++)
	{
		controlPackage[packageIndex++] = types[index];

		controlPackage[packageIndex++] = lengths[index];
		
		memcpy(controlPackage + packageIndex, (unsigned char *) values[index], lengths[index]);

		packageIndex += lengths[index];
	}
	if( llWrite(file_descriptor, controlPackage, packageIndex) == -1)
		return -1;
	else
		return 0;
}
