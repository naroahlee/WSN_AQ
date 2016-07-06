#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "cyclic_util.h"
#include "wsn_aqm.h"


int32_t main(int32_t argc, char* argv[])
{
	FILE* fpIn;
	aq_record   strItem;
	char acbuff[20];

	if(argc != 2)
	{
		fprintf(stderr, "Argument Error!\n");
		exit(EXIT_FAILURE);
	}

	if(NULL == (fpIn = fopen(argv[1], "rb")))
	{
		fprintf(stderr, "File Open Error!\n");
		exit(EXIT_FAILURE);
	}

	while(1 == fread((void*)&strItem, sizeof(aq_record), 1, fpIn))
	{
		strftime(acbuff, 20, "%Y-%m-%d %H:%M:%S", localtime(&strItem.strtime));
		printf("[%s] NdID[%2d] PkID[%3d] Data[%4d]\n",
			acbuff,
			strItem.strdata.u8ndidx,
			strItem.strdata.u8fmidx,
			strItem.strdata.u16data);
	}

	fclose(fpIn);
	return 0;
}
