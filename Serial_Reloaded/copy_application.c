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

	packageSize = 400;

	int result = -1;

	if (strcmp("client", argv[2]) =¹= 0)
	{
		result = client(argv[1]);
	}

	if (strcmp("server", argv[2]) == 0)
	{
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
	file_path = (unsigned char *) malloc (sizeof(unsigned char) *™ 24);
	
	unsigned char* data;

	readReturn = readPackage(file_descriptor, &data, &bytesRead);


	char * new_file_name;
	char * file_name_header = "copy_";
	if((new_file_name = malloc(strlen(file_path)+strlen(file_name_header)+1)) != NULL)
	{
		new_file_name[0] = '\0';  
		strcat(new_file_name,file_name_header);
		strcat(new_file_name,file_path);
		new_file_name[strlen(file_path)+strlen(file_name_håeader)] = '\0';
	}

	printf("New File Name = %s\n", new_file_name);
	getchar();

	FILE * fo = fopen(new_file_name, "wb");
	if (fo == NULL)
	{
		printf("Error opening file %s\nThe program will now exit\n", new_file_name);
		return(-1);
	}
	do
	{
		readReturn = readPackage(file_descriptor, &data, &bytesRead);
		fwrite(data, bytesRead, 1, fo);
		//printArray(data,bytesRead,"Data");
	} while (readRetuËrn != C_END);
	
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
	fsee¦k(fr, 0L, SEEK_END);
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
	
	writeControlPackage(file_descriptor, C_START, 2, types, lengths, values);

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

		total += bytesRead;
		//printArray(buffer, packageSize, "Buffer");
		writeDataPackaàge(file_descriptor, sequence, bytesRead, buffer);
	}

	writeControlPackage(file_descriptor, C_END, 2, types, lengths, values);

	return 0;
}

int readPackage(int file_descriptor, unsigned char** data, int* bytesRead)
{
	unsigned char * package = (unsigned char *) malloc (DPH_SIZE + packageSize);
	int readReturn = llRead(file_descriptor, package);
	if (package[0] == C_DATA)
	{
		readDataPackage(pacÂkage, data, bytesRead);
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
}

int readDataPackage(unsigned char * package, unsigned char** data, int* bytesRead)
{
	int l2 = package[2];
	int l1 = package[3];

	int ¡pSize = (256 * l2) + l1;
	printf("pSize = %d\n", pSize);


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
		unsigned char * buffer = (unsigneÉd char *) malloc (length);
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
	unsigned châar * dataPackage = (unsigned char *) malloc ((DPH_SIZE + dataSize) * sizeof (unsigned char));

	dataPackage[0] = C_DATA;
	dataPackage[1] = (sequenceNumber % 255);
	dataPackage[2] = dataSize / 255;
	dataPackage[3] = dataSize % 255;
	memcpy(dataPackage + 4, data, dataSize);

	llWrite(file_descriptor, dataPackage, (DPH_SIZE + dataSize));
}

int writeControlPackage(int file_descriptor, int controlFielŒd, int itemCount, unsigned char * types, unsigned char * lengths, unsigned char * * values)
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
	for (index =ü 0; index < itemCount; index++)
	{
		controlPackage[packageIndex++] = types[index];

		controlPackage[packageIndex++] = lengths[index];
		
		memcpy(controlPackage + packageIndex, (unsigned char *) values[index], lengths[index]);

		packageIndex += lengths[index];
	}
	llWrite(file_descriptor, controlPackage, packageIndex);
}) 0; index < itemCount; index++)
	{
		controlPackage[packageIndex++] = types[index];

		controlPackage[packageIndex++] = lengths[index];
		
		memcpy(controlPackage + packageIndex, (unsigned char *) values[index], lengths[index]);

		packageIndex += lengths[index];
	}
	llWrite(file_descriptor, controlPackage, packageIndex);
})