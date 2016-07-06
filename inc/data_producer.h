
#ifndef __DATA_PRODUCER_H__
#define __DATA_PRODUCER_H__

/* Serial System */
#define MODULE_DEV "/dev/ttyAMA0" 
#define MODULE_BAUD (9600)

int32_t start_producer(struct xbee** ppstrxbee, struct xbee_con** ppstrcon);
int32_t stop_producer(struct xbee* pstrxbee, struct xbee_con *pstrcon);
#endif
