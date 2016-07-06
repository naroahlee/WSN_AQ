#ifndef __CYCLIC_UTIL_H__
#define __CYCLIC_UTIL_H__

#define MAX_RECORD_LENGTH (1024)

typedef struct data_frame_t
{
    uint8_t  u8ndidx;
    uint8_t  u8fmidx;
	uint16_t u16data;
} data_frame;

typedef struct aq_record_t
{
	time_t	   strtime;
	data_frame strdata;
} aq_record;

typedef struct aq_cyclist_t 
{
	int32_t s32rear;
	int32_t s32front;
	aq_record strList[MAX_RECORD_LENGTH];	
} aq_cyclist;

int32_t addOneRecord(aq_cyclist* pstrList, aq_record* pstrItem);
int32_t getOneRecord(aq_cyclist* pstrList, aq_record* pstrItem);
#endif

