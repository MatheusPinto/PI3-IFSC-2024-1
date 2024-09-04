#include <stdint.h>
#include "main.h"
#include <stdio.h>

#include "timing.h"

#define TIMING_TOTAL_ENABLED (1 << 0)
#define TIMING_INTERVAL_ENABLED (1 << 1)
#define TIMING_REFRESH_ENABLED (1 << 2)

static uint32_t timingStatus;

static uint32_t timingTotal;
static uint32_t timingInterval;
static uint32_t timingIntervalCurr;
static uint32_t timingRefresh;

void timingInit()
{
    timingStatus = 0;
    timingTotal = TIMING_TOTAL_DEFAULT;
    timingInterval = TIMING_INTERVAL_DEFAULT;
    timingRefresh = TIMING_REFRESH;
    timingIntervalCurr = 0;
}

void timingUpdate(void)
{
    if (timingStatus & TIMING_INTERVAL_ENABLED)
    {
        if (timingIntervalCurr >= timingInterval)
            timingIntervalCurr = 0;
        else
            timingIntervalCurr += FSM_BASE_TIME_MS;
    }
    if (timingStatus & TIMING_TOTAL_ENABLED)
    {
        if (timingTotal > 0)
            timingTotal -= FSM_BASE_TIME_MS;
    }
    if (timingStatus & TIMING_REFRESH_ENABLED)
    {
        if (timingRefresh > 0)
            timingRefresh -= FSM_BASE_TIME_MS;
        else
            timingRefresh = TIMING_REFRESH;
    }
}

uint8_t timingTotalDone(void)
{
    return timingTotal == 0;
}

uint8_t timingIntervalDone(void)
{
    return timingIntervalCurr >= timingInterval;
}

uint8_t timingRefreshDone(void)
{
    return timingRefresh == 0;
}

void timingTotalEnable(uint8_t flag)
{
    if (flag)
        timingStatus |= TIMING_TOTAL_ENABLED;
    else
        timingStatus &= ~TIMING_TOTAL_ENABLED;
}
void timingIntervalEnable(uint8_t flag)
{
    if (flag && timingInterval > 0)
        timingStatus |= TIMING_INTERVAL_ENABLED;
    else
        timingStatus &= ~TIMING_INTERVAL_ENABLED;
}
void timingRefreshEnable(uint8_t flag)
{
    if (flag)
        timingStatus |= TIMING_REFRESH_ENABLED;
    else
        timingStatus &= ~TIMING_REFRESH_ENABLED;
}

void timingIntervalSet(uint32_t val)
{
    if (val < TIMING_INTERVAL_MIN)
        val = TIMING_INTERVAL_MIN;
    else if (val > TIMING_INTERVAL_MAX)
        val = TIMING_INTERVAL_MAX;
    timingInterval = val;
}
void timingIntervalInc()
{
    timingIntervalSet(timingInterval + TIMING_INTERVAL_STEP);
}
void timingIntervalDec()
{
    timingIntervalSet(timingInterval - TIMING_INTERVAL_STEP);
}
const char *timingIntervalStr(void)
{
    static char str[5] = "muuu";
    snprintf(str, 5, "%dmin", (uint8_t)(timingInterval / 60000));
    return str;
}

void timingTotalSet(uint32_t val)
{
    if (val < TIMING_TOTAL_MIN)
        val = TIMING_TOTAL_MIN;
    else if (val > TIMING_TOTAL_MAX)
        val = TIMING_TOTAL_MAX;
    timingTotal = val;
}
void timingTotalInc()
{
    timingTotalSet(timingTotal + TIMING_TOTAL_STEP);
}
void timingTotalDec()
{
    timingTotalSet(timingTotal - TIMING_TOTAL_STEP);
}
const char *timingTotalStr(void)
{
    static char str[6] = "H?mmh";
    snprintf(str, 6, "%d:%02dh", (uint8_t) (timingTotal / 60000 / 60), (uint8_t) (timingTotal / 60000)%60);
    return str;
}
