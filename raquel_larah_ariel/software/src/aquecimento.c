#include <stdint.h>
#include <avr/io.h>

#include "aquecimento.h"
#include <stdio.h>
#include "lcd.h"

static uint16_t aquecimentoTarget;
static aquecimentoStatusType aquecimentoStatus;

static void aquecimentoLiga(void)
{
    AQUECIMENTO_PORT |= AQUECIMENTO_MASK;
}

static void aquecimentoDesliga(void)
{
    AQUECIMENTO_PORT &= ~AQUECIMENTO_MASK;
}

void aquecimentoInit()
{
    aquecimentoTarget = AQUECIMENTO_TARGET_TEMP_DEFAULT;
    aquecimentoStatus = AQUECIMENTO_DISABLED;

    AQUECIMENTO_PORT &= ~AQUECIMENTO_MASK;
    AQUECIMENTO_DDRR |= AQUECIMENTO_MASK;

    aquecimentoDesliga();
}

void aquecimentoEnable()
{
    aquecimentoStatus = AQUECIMENTO_ENABLED;
}

void aquecimentoDisable()
{
    aquecimentoStatus = AQUECIMENTO_DISABLED;
    aquecimentoDesliga();
}

void aquecimentoUpdate(uint16_t temperature)
{
    if (aquecimentoStatus == AQUECIMENTO_DISABLED)
        aquecimentoStatus = AQUECIMENTO_DISABLED;
    else if (temperature <= aquecimentoTarget)
        aquecimentoStatus = AQUECIMENTO_HEATING;
    else if (temperature >= aquecimentoTarget + 5)
        aquecimentoStatus = AQUECIMENTO_COOLING;
	
    switch (aquecimentoStatus)
    {
    case AQUECIMENTO_HEATING:
        aquecimentoLiga();
        break;
	case AQUECIMENTO_ENABLED:
    case AQUECIMENTO_COOLING:
    case AQUECIMENTO_DISABLED:
        aquecimentoDesliga();
        break;
    }
}

aquecimentoStatusType aquecimentoStatusGet()
{	
    return aquecimentoStatus;
}

void aquecimentoTargetSet(uint16_t temperature)
{
    if (temperature < AQUECIMENTO_TARGET_TEMP_MIN)
        temperature = 0;
    else if (temperature > AQUECIMENTO_TARGET_TEMP_MAX)
        temperature = AQUECIMENTO_TARGET_TEMP_MAX;
    aquecimentoTarget = temperature;
}

uint16_t aquecimentoTargetGet()
{
    return aquecimentoTarget;
}

void aquecimentoTargetInc()
{
    if (aquecimentoTarget == 0)
        aquecimentoTargetSet(AQUECIMENTO_TARGET_TEMP_MIN);
    else
        aquecimentoTargetSet(aquecimentoTarget + AQUECIMENTO_TARGET_TEMP_STEP);
}

void aquecimentoTargetDec()
{
    if (aquecimentoTarget >= AQUECIMENTO_TARGET_TEMP_MIN)
        aquecimentoTargetSet(aquecimentoTarget - AQUECIMENTO_TARGET_TEMP_STEP);
}

const char *aquecimentoTargetStr()
{
    static char str[5] = "1234";
    snprintf(str, 5, "%02d%cC", aquecimentoTarget, 0xDF);
    return str;
}