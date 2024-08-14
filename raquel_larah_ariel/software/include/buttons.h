#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <stdint.h>
#include "main.h"

#define BUTTONS_UP_PORT PORTB
#define BUTTONS_UP_DDR DDRB
#define BUTTONS_UP_PINR PINB
#define BUTTONS_UP_PCMSK PCMSK0
#define BUTTONS_UP_PCICR_MASK (1 << PCIE0)
#define BUTTONS_UP_MASK (1 << PINB1)

#define BUTTONS_DW_PORT PORTB
#define BUTTONS_DW_DDR DDRB
#define BUTTONS_DW_PINR PINB
#define BUTTONS_DW_PCMSK PCMSK0
#define BUTTONS_DW_PCICR_MASK (1 << PCIE0)
#define BUTTONS_DW_MASK (1 << PINB2)

#define BUTTONS_OK_PORT PORTB
#define BUTTONS_OK_DDR DDRB
#define BUTTONS_OK_PINR PINB
#define BUTTONS_OK_PCMSK PCMSK0
#define BUTTONS_OK_PCICR_MASK (1 << PCIE0)
#define BUTTONS_OK_MASK (1 << PINB3)

#define BUTTONS_HOLD_TIME_MS 1000
#define BUTTONS_BURST_TIME_MS 500

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "main.h"
#include "buttons.h"

#define BUTTONS_OK() ((BUTTONS_OK_PINR & BUTTONS_OK_MASK))
#define BUTTONS_DW() ((BUTTONS_DW_PINR & BUTTONS_DW_MASK))
#define BUTTONS_UP() ((BUTTONS_UP_PINR & BUTTONS_UP_MASK))

uint8_t buttons_state_Apply();
void buttons_state_Next();
void buttons_Init();

#endif