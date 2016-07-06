#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cyclic_util.h"

int32_t addOneRecord(aq_cyclist* pstrList, aq_record* pstrItem)
{
	
	memcpy((void*)&pstrList->strList[pstrList->s32front],
		   (void*)pstrItem,
		   sizeof(aq_record));
	pstrList->s32front = ((pstrList->s32front + 1) % MAX_RECORD_LENGTH);
	if(pstrList->s32front == pstrList->s32rear)
	{
		fprintf(stderr, "Buffer FULL!\n");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}

int32_t getOneRecord(aq_cyclist* pstrList, aq_record* pstrItem)
{
	if(pstrList->s32rear == pstrList->s32front)
	{
		/* Buffer Empty */
		return -1;
	}
	memcpy((void*)pstrItem,
		   (void*)&pstrList->strList[pstrList->s32rear],
		   sizeof(aq_record));
	pstrList->s32rear = ((pstrList->s32rear + 1) % MAX_RECORD_LENGTH);
	return 0;
}
