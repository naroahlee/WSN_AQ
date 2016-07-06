#ifndef __DATA_CONSUMER_H__
#define __DATA_CONSUMER_H__

#define FILE_PATH		"./Data/"
#define FILE_POSTFIX	".dat"
#define FILE_DUMP_TIME (3 * 30)

void start_consumer(int32_t s32invsec);
void stop_consumer(void);

#endif
