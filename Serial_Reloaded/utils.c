void printArray(unsigned char * array, int arraySize, char* arrayName)
{
	int index;

	for (index = 0; index < arraySize; index ++)
	{
		printf("%s[%d]\t%X\t%c\t%d\n", arrayName, index, array[index], array[index], array[index]);
	}
}