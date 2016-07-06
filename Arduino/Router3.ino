// Work.ino
// For Forming a dummy structure and sending them to coodinator

#include <SoftwareSerial.h>
#include <MsTimer2.h>
#include <stdlib.h>
#include <string.h>

#define XBEE_START (0x7E)
#define XBEE_START_OFFSET (0)
#define XBEE_START_OFFSET (0)
#define XBEE_LEN_OFFSET (1)
#define XBEE_FRAMETYPE_OFFSET (3)
#define XBEE_FRAMEID_OFFSET (4)
#define XBEE_16ADDR_OFFSET (13)
#define XBEE_FRAMETYPE_TXRQ (0x10)
#define XBEE_PAYLOAD_OFFSET (17)
#define XBEE_LENGTH_BASE (14)

#define WSN_ROUTER_ID (3)
/* Router1: ABoard + BigBread */
#if WSN_ROUTER_ID == 1
#define XBEE_DOUT_PIN  (4)
#define XBEE_DIN_PIN   (2)
#define AQSEN_LED_PIN  (3)
/* Analog Pin */
#define AQSEN_ARD_PIN  (2)
#define XBEE_DUMMY_DATA (0xAAAA)
/* y = Kx + B */
#define CORR_K         (1.0)
#define CORR_B         (0.0)

/* Router2: ABoard + Daughter Card */
#elif WSN_ROUTER_ID == 2
#define XBEE_DOUT_PIN (13)
#define XBEE_DIN_PIN (12)
#define AQSEN_LED_PIN  (3)
/* Analog Pin */
#define AQSEN_ARD_PIN  (2)
#define XBEE_DUMMY_DATA (0xBBBB)
#define CORR_K         (0.2033)
#define CORR_B         (67.7377)

/* Router3: ABoard + SmallBread */
#elif WSN_ROUTER_ID == 3
#define XBEE_DOUT_PIN  (4)
#define XBEE_DIN_PIN   (2)
#define AQSEN_LED_PIN  (3)
/* Analog Pin */
#define AQSEN_ARD_PIN  (2)
#define XBEE_DUMMY_DATA (0x5555)
#define CORR_K         (0.3219)
#define CORR_B         (71.2240)

#else
/* NOT AVAILIABLE */
#endif

#define AQ_SAMPLE_PERIOD (1000)
#define FRAME_BUF_MAX (100)

typedef struct data_frame_t
{
	uint8_t  u8ndidx;
	uint8_t  u8fmidx;
	uint16_t u16data;
} data_frame;

// This Global Buffer is used for storing the formed frame
// A global counter is resiponsible for setting the cnt
uint8_t gau8fb[FRAME_BUF_MAX] = {0};
uint8_t gu8fbcnt = 0;

SoftwareSerial xbee(XBEE_DOUT_PIN, XBEE_DIN_PIN);

/* This will form a Tx frame whose dest is coordinator */
uint16_t form_tx_frame(uint16_t u16dst, uint8_t u8fbno, data_frame* pstrfm)
{
	uint16_t u16len;
	uint16_t u16total;
	int16_t s16i;
	/* Set Start */
	gau8fb[XBEE_START_OFFSET] = XBEE_START;
	/* Set Length */
	u16len = XBEE_LENGTH_BASE + sizeof(data_frame);
	u16total = u16len + 4;
	gau8fb[XBEE_LEN_OFFSET]     = (uint8_t)((u16len >> 8) & 0xFF);
	gau8fb[XBEE_LEN_OFFSET + 1] = (uint8_t)((u16len     ) & 0xFF);
	/* Set Frametype */
	gau8fb[XBEE_FRAMETYPE_OFFSET] = XBEE_FRAMETYPE_TXRQ;
	/* Set FrameID */
	gau8fb[XBEE_FRAMEID_OFFSET] = u8fbno;
	/* Set 16ADDR */
	gau8fb[XBEE_16ADDR_OFFSET]     = (uint8_t)((u16dst >> 8) & 0xFF);
	gau8fb[XBEE_16ADDR_OFFSET + 1] = (uint8_t)((u16dst     ) & 0xFF);
	/* Set Payload */
	memcpy((void *)&gau8fb[XBEE_PAYLOAD_OFFSET], (void *)pstrfm, sizeof(data_frame));

	gau8fb[u16total - 1] = 0xFF;
	for(s16i = XBEE_FRAMETYPE_OFFSET; s16i < u16total - 1; s16i++)
	{
		gau8fb[u16total - 1] -= gau8fb[s16i];
	}


	return u16total;
}



void setup() 
{
	pinMode(AQSEN_LED_PIN, OUTPUT);  
	Serial.begin(9600);
	xbee.begin(9600);
	MsTimer2::set(AQ_SAMPLE_PERIOD, timer_hndler);
	MsTimer2::start();
}

/* Read Analog Data           */
/* We need to control the LED */
uint16_t rd_sensor_analog_data(void)
{
	uint16_t u16dustval;
	/* Pull current drive, LOW -> Light */
	digitalWrite(AQSEN_LED_PIN, LOW);
	delayMicroseconds(280);
	u16dustval = analogRead(AQSEN_ARD_PIN);
	delayMicroseconds(40);
	/* Turn off the LED */
	digitalWrite(AQSEN_LED_PIN, HIGH);
	return u16dustval;
}

void timer_hndler()
{
	int s16i;
	int u16total;
  int u16tmp;
	data_frame strcap;
	gu8fbcnt++;
	/* Read Analog Data */
	strcap.u8ndidx = WSN_ROUTER_ID;
	strcap.u8fmidx = gu8fbcnt;
  u16tmp = rd_sensor_analog_data();
	strcap.u16data = (uint16_t)(CORR_K * u16tmp + CORR_B);
	//strcap.u16data = XBEE_DUMMY_DATA;

	/* Inc the Counter */
	u16total = form_tx_frame(0xFFFE, gu8fbcnt, &strcap);
	/* Send Data */
	for(s16i = 0; s16i < u16total; s16i++)
	{
		xbee.write(gau8fb[s16i]);
	}
	return;
}

void loop()
{
  // put your main code here, to run repeatedly:
	return;
}
