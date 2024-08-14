#ifndef TERMOMETRO_H_INCLUDED
#define TERMOMETRO_H_INCLUDED

#include <stdint.h>
#include <avr/io.h>

#include "main.h"

#define TERMO_PORT PORTB
#define TERMO_DDR DDRB
#define TERMO_PORTIN PINB
#define TERMO_MASK (1 << PORTB0)

/* Passa adiante os erros definidos na biblioteca dsb18b20 para manter a nomenclatura padrão*/
#define TERMO_ERROR_COMM DS18B20_ERROR_COMM   //!< Communication with sensor failed
#define TERMO_ERROR_CRC DS18B20_ERROR_CRC     //!< Data CRC check failed
#define TERMO_ERROR_PULL DS18B20_ERROR_PULL   //!< Received only zeros - you may want to check the 1wire bus pull-up resistor
#define TERMO_ERROR_OTHER DS18B20_ERROR_OTHER //!< Other reason (bad user-provided argument, etc.)
#define TERMO_ERROR_INVALID_STATE 5           //!< Internal logical error (probably a bug in FSM logic)

uint8_t termo_state_Apply();
void termo_state_Next();

#endif
