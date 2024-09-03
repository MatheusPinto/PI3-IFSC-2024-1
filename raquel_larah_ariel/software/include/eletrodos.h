#ifndef ELETRODOS_H_INCLUDED
#define ELETRODOS_H_INCLUDED

#include <avr/io.h>

#define ELETRODOS_E_PORT PORTC
#define ELETRODOS_E_DDRR DDRC
#define ELETRODOS_E_PINR PINC
#define ELETRODOS_E_MASK (1 << PINC5)

#define ELETRODOS_P_PORT PORTC
#define ELETRODOS_P_DDRR DDRC
#define ELETRODOS_P_PINR PINC
#define ELETRODOS_P_MASK (1 << PINC4)

#define ELETRODOS_N_PORT PORTC
#define ELETRODOS_N_DDRR DDRC
#define ELETRODOS_N_PINR PINC
#define ELETRODOS_N_MASK (1 << PINC3)

void eletrodosInit();
void eletrodosOff();
void eletrodosPos();
void eletrodosNeg();
void eletrodosInv();

#endif

