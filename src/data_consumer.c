#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include "wsn_aqm.h"
#include "cyclic_util.h"
#include "data_consumer.h"

FILE* gfpOut;
int32_t gs32fwrcnt = 0;
char gacFileName[40];
#define FILE_PATH		"./Data/"
#define FILE_POSTFIX	".dat"


void setFileName(void)
{
	char acbuff[20];
	time_t strnow;
	time(&strnow);
	strftime(acbuff, 20, "%Y%m%d_%H%M%S", localtime(&strnow));
	sprintf(gacFileName, "%s%s%s", FILE_PATH, acbuff, FILE_POSTFIX);
	return;
}

void consume_data(int32_t s32signo, siginfo_t* pstrext, void* pucontext)
{
	int32_t s32ret;
	aq_cyclist* pstrList;
	aq_record   strItem;
	pstrList = getgstrRecBufptr();
	while(1)
	{
		s32ret = getOneRecord(pstrList, &strItem);
		
		if(0 != s32ret)
		{
			break;
		} 
		else
		{
			fwrite((void*)&strItem, sizeof(aq_record), 1, gfpOut);
		}
	}
	//printf("Write File %s!\n", gacFileName);


	gs32fwrcnt++;	
	if(gs32fwrcnt >= FILE_DUMP_TIME)
	{
		/* Finish Writing a file */
		fclose(gfpOut);
		gs32fwrcnt = 0;
		printf("Close Old File %s\n", gacFileName);

		setFileName();
		/* open and writing a new file */
		if(NULL == (gfpOut = fopen(gacFileName, "wb")))
		{
			fprintf(stderr, "File Open Error!\n");
			exit(EXIT_FAILURE);
		}
	}
		
	return;
}

void start_consumer(int32_t s32invsec)
{
	struct itimerspec strtspc;
	struct timespec strnow;
	timer_t strtimer;
	struct sigaction sa;
	struct sigevent strtevn;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = &consume_data;

	strtspc.it_interval.tv_sec  = s32invsec;
	strtspc.it_interval.tv_nsec = 0;

	/* Wall Time */
	if(clock_gettime(CLOCK_REALTIME, &strnow) < 0)
	{
		perror("clock_gettime()");
		exit(EXIT_FAILURE);
	}

	/* Set Start Time */
	strtspc.it_value.tv_sec = strnow.tv_sec + s32invsec;
	strtspc.it_value.tv_nsec = 0;

	strtevn.sigev_notify = SIGEV_SIGNAL;
	strtevn.sigev_signo  = SIGRTMIN;
	strtevn.sigev_value.sival_ptr = (void*)&strtimer;

	if(timer_create(CLOCK_REALTIME, &strtevn, &strtimer) < 0)
	{
		perror("timer_create()");
		exit(EXIT_FAILURE);
	}

	if(timer_settime(strtimer, TIMER_ABSTIME, &strtspc, NULL) < 0)
	{
		perror("timer_settime");
		exit(EXIT_FAILURE);
	}

	/* Bind the data_consumer action */
	if(sigaction(SIGRTMIN, &sa, NULL) < 0)
	{
		perror("sigaction Err");
		exit(EXIT_FAILURE);
	}


	setFileName();

	if(NULL == (gfpOut = fopen(gacFileName, "wb")))
	{
		perror("File Open Err");
		exit(EXIT_FAILURE);	
	}

	return;
}

void stop_consumer(void)
{
	fclose(gfpOut);
	return;
}
