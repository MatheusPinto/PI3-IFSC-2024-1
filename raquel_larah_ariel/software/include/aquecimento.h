#ifndef AQUECIMENTO_H_INCLUDED
#define AQUECIMENTO_H_INCLUDED

#include <stdint.h>
#include <avr/io.h>

#define AQUECIMENTO_PORT PORTC
#define AQUECIMENTO_DDRR DDRC
#define AQUECIMENTO_PINR PINC
#define AQUECIMENTO_MASK (1 << PINC2)

#define AQUECIMENTO_TARGET_TEMP_DEFAULT 25
#define AQUECIMENTO_TARGET_TEMP_MIN 20
#define AQUECIMENTO_TARGET_TEMP_MAX 99
#define AQUECIMENTO_TARGET_TEMP_STEP 1

typedef enum
{
    AQUECIMENTO_HEATING = 0,
    AQUECIMENTO_COOLING,
    AQUECIMENTO_DISABLED,
	AQUECIMENTO_ENABLED,
} aquecimentoStatusType;

void aquecimentoInit();
void aquecimentoEnable();
void aquecimentoDisable();
void aquecimentoUpdate(uint16_t temperature);
aquecimentoStatusType aquecimentoStatusGet();
void aquecimentoTargetSet(uint16_t temperature);
uint16_t aquecimentoTargetGet();
void aquecimentoTargetInc();
void aquecimentoTargetDec();
const char *aquecimentoTargetStr();

#endif
