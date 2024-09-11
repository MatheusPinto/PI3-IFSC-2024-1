#ifndef TIMING_H_INCLUDED
#define TIMING_H_INCLUDED

#include <stdint.h>

/*******************************TEMPO******************************************/
#define TIMING_TOTAL_DEFAULT (120 * 60UL * 1000)
#define TIMING_TOTAL_MIN (30 * 60UL * 1000)
#define TIMING_TOTAL_MAX (120 * 60UL * 1000)
#define TIMING_TOTAL_STEP (10 * 60UL * 1000)
/*******************************INTERVALO**************************************/
#define TIMING_INTERVAL_DEFAULT (4 * 60UL * 1000)
#define TIMING_INTERVAL_MIN (0 * 60UL * 1000)
#define TIMING_INTERVAL_MAX (9 * 60UL * 1000)
#define TIMING_INTERVAL_STEP (1 * 60UL * 1000)
/******************************************************************************/
#define TIMING_REFRESH (1 * 1000)

void timingInit();
void timingUpdate(void);
uint8_t timingTotalDone(void);
uint8_t timingIntervalDone(void);
uint8_t timingRefreshDone(void);
void timingTotalEnable(uint8_t flag);
void timingIntervalEnable(uint8_t flag);
void timingRefreshEnable(uint8_t flag);
void timingIntervalSet(uint32_t val);
void timingIntervalInc();
void timingIntervalDec();
const char *timingIntervalStr(void);
void timingTotalSet(uint32_t val);
void timingTotalInc();
void timingTotalDec();
const char *timingTotalStr(void);

#endif
