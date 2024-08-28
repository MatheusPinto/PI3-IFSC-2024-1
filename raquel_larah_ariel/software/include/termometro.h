#ifndef TERMOMETRO_H_INCLUDED
#define TERMOMETRO_H_INCLUDED

#include <stdint.h>
#include <avr/io.h>

#define TERMO_PORT PORTB
#define TERMO_DDR DDRB
#define TERMO_PORTIN PINB
#define TERMO_MASK (1 << PORTB0)

typedef enum
{
    TERMO_UNINIT,
    TERMO_BUSY,
    TERMO_READY,
    TERMO_ERROR_COM,
    TERMO_ERROR_PULL,
    TERMO_ERROR_CRC,
} termo_result;

termo_result termo_conv();
termo_result termo_read(int16_t *temperatura);
void termo_update();

#endif
