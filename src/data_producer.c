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
#include "wsn_aqm.h"

/* ===================================================== */
void produce_data(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt, void **data)
{
	aq_record strrec;

	if(sizeof(data_frame) != (*pkt)->dataLen)
	{
		printf("df[%d] vs len[%d]\n", sizeof(data_frame), (*pkt)->dataLen);
		printf("Unexpected Data Rcved! Discard!\n");
		return;
	}
	/* Fetch Data to local */
	memcpy((void *)&strrec.strdata, 
		   (void *)((*pkt)->data),
		   (sizeof(data_frame)));
	time(&strrec.strtime);
	/* Save into Buffer */
	(void)addOneRecord(getgstrRecBufptr(), &strrec);
/*
	printf("[NdID = %2d] [PkID = %3d]: %d\n",
			strrec.strdata.u8ndidx,
			strrec.strdata.u8fmidx,
			strrec.strdata.u16data);
*/

	return;
}

int32_t start_producer(struct xbee** ppstrxbee, struct xbee_con** ppstrcon)
{
	int32_t s32ret;
	struct xbee *pstrxbee;
	struct xbee_con *pstrcon;
	struct xbee_conAddress straddr;
	struct xbee_conSettings strconset;

	pstrxbee = *ppstrxbee;
	pstrcon  = *ppstrcon;

	s32ret = xbee_setup(&pstrxbee, "xbee2", MODULE_DEV, MODULE_BAUD);
	if(XBEE_ENONE != s32ret)
	{
		printf("ret: %d (%s)\n", s32ret, xbee_errorToStr(s32ret));
		return s32ret;
	}

	/* I had to provide addr here Potential Issue */
	memset(&straddr, 0, sizeof(struct xbee_conAddress));
	/* I had to setup the addr64_enabled */
	straddr.addr64_enabled = 1;
	straddr.addr64[0] = 0x00;
	straddr.addr64[1] = 0x00;
	straddr.addr64[2] = 0x00;
	straddr.addr64[3] = 0x00;
	straddr.addr64[4] = 0x00;
	straddr.addr64[5] = 0x00;
	straddr.addr64[6] = 0xFF;
	straddr.addr64[7] = 0xFF;
	/* Bulid up connection */	
	s32ret = xbee_conNew(pstrxbee, &pstrcon, "Data", &straddr);
	if(XBEE_ENONE != s32ret)
	{
		printf("xbee_conNew() ret: %d (%s)\n", s32ret, xbee_errorToStr(s32ret));
		return s32ret;
	}

    xbee_conSettings(pstrcon, NULL, &strconset);
    strconset.catchAll = 1;
    xbee_conSettings(pstrcon, &strconset, NULL);
	

	s32ret = xbee_conCallbackSet(pstrcon, produce_data, NULL);
	if(XBEE_ENONE != s32ret)
	{
		printf("xbee_conCallbackSet() ret: %d (%s)\n", s32ret, xbee_errorToStr(s32ret));
		return s32ret;
	}

	*ppstrxbee = pstrxbee;
	*ppstrcon  = pstrcon;
	return 0; 
}

int32_t stop_producer(struct xbee* pstrxbee, struct xbee_con *pstrcon)
{
	int32_t s32ret;
	/* Release Con */
	s32ret = xbee_conEnd(pstrcon);
	if(XBEE_ENONE != s32ret)
	{
		printf("ret: %d (%s)\n", s32ret, xbee_errorToStr(s32ret));
		return s32ret;
	}

	/* Release xbee */
	xbee_shutdown(pstrxbee);
	return s32ret;
}
