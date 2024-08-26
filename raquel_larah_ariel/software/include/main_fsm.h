#ifndef MAIN_FSM_H_INCLUDED
#define MAIN_FSM_H_INCLUDED

#include <stdint.h>

#define TEMPERATURA_MINIMA 50
#define TEMPERATURA_MAXIMA 99
#define TEMPERATURA_PASSO 1

#define TEMPO_MINIMO 30
#define TEMPO_MAXIMO 120
#define TEMPO_PASSO 10

uint8_t main_state_Apply(void);
void main_state_Next(void);

extern volatile uint16_t main_timer;
#endif
