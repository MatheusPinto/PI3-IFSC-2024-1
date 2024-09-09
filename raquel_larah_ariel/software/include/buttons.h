#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <avr/io.h>
#include <stdint.h>

#define BUTTONS_OK_PORT PORTD
#define BUTTONS_OK_DDR DDRD
#define BUTTONS_OK_PINR PIND
#define BUTTONS_OK_MASK (1 << PIND5)

#define BUTTONS_DOWN_PORT PORTD
#define BUTTONS_DOWN_DDR DDRD
#define BUTTONS_DOWN_PINR PIND
#define BUTTONS_DOWN_MASK (1 << PIND6)

#define BUTTONS_UP_PORT PORTD
#define BUTTONS_UP_DDR DDRD
#define BUTTONS_UP_PINR PIND
#define BUTTONS_UP_MASK (1 << PIND7)

#define BUTTONS_HOLD_TIME_MS 1000
#define BUTTONS_BURST_TIME_MS 500

void buttonsInit();
void buttonsUpdate();
uint8_t buttonsUp();
uint8_t buttonsDown();
uint8_t buttonsOk();
void buttonsDownBurst(uint8_t flag);
void buttonsUpBurst(uint8_t flag);

#endif
