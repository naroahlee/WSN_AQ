#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <xbee.h>
#include "cyclic_util.h"
#include "data_producer.h"
#include "data_consumer.h"
#include "wsn_aqm.h"

aq_cyclist gstrRecBuf = 
{
	.s32front = 0,
	.s32rear = 0,
}; 

aq_cyclist* getgstrRecBufptr(void)
{
	return &gstrRecBuf;
}

/* ================= Ctrl-C Exit Signal ================ */
int32_t gs32exitflag = 0;
void set_exitflag(int32_t s32flag)
{
	gs32exitflag = s32flag;
	return;
}


static void my_sighnl(int32_t signo)
{
	if(SIGINT == signo)
	{
		printf("\nCatch SIGINT, Exit!\n");
		set_exitflag(1);
	}	
	return;
}


int32_t main(int32_t argc, char* argv[])
{
	struct xbee *pstrxbee;
	struct xbee_con *pstrcon;

	/* Setup Exit */
	if(SIG_ERR == signal(SIGINT, my_sighnl))
	{
		fprintf(stderr, "Cannot catch SIGINT!\n");
		exit(EXIT_FAILURE);
	}
	
		
	(void)start_producer(&pstrxbee, &pstrcon);
	/* Init a Periodic Timer */
	(void)start_consumer(DATA_CONSUME_INTERVAL);

	/* Main Loop */
	while(!gs32exitflag)
	{
		sleep(5);
	}

	(void)stop_consumer();
	(void)stop_producer(pstrxbee, pstrcon);

	return 0;
}
