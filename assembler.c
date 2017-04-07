#include "common.h"

/* the main function recieves arguments for file names (without ".as" at the end), and passes each of them through the first loop and then the second loop */
int main(int argc, char *argv[]) 
{
	int i;
	int errorCounter = 0;
	char * fileName;
	
	for(i=1;i<argc;i++)
	{
		fileName = (char *)malloc(MAX_FILENAME * sizeof(char));
		strcpy(fileName,argv[i]);
		startLoop(fileName, errorCounter);
		if(errorCounter == 0)
		{
			endLoop(fileName);
		}
		else
		{
			printf("There were %d errors in file %s, no files created/n",errorCounter,fileName);
		}
	}

	return 0;
}

