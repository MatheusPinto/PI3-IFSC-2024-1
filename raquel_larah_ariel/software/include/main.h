#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED
#include <stdint.h>

#define FSM_BASE_TIME_MS 1UL

/* Indica que a maquina de estados está esperando por um dos eventos */
#define FSM_EV_WAIT 0
/* Indica que a maquina de estados não está esperando por um evento */
#define FSM_CONTINUE 1

#define EV_BT_ERROR (1 << 0)
#define EV_BT_EXT (1 << 1)
#define EV_BT_DOWN (1 << 2)
#define EV_BT_OK (1 << 3)
#define EV_BT_TIMER (1 << 4)
#define EV_BT_UP (1 << 5)

#define EV_TERMO_TIMER (1 << 6)
#define EV_TERMO_ERROR (1 << 7)
#define EV_TERMO_NEW (1 << 8)

#define EV_MAIN_TIMER (1 << 9)

typedef uint8_t (*const FSM_applyType)(void);

/* Variável utilizada para ler quais eventos ocorreram desde a última passagem pelo loop principal */
extern const volatile uint16_t *const ev_flags;
/* Emite um evento */
void ev_write(uint16_t mask);

extern int16_t termo_temperatura;
extern uint8_t termo_error;

extern volatile uint16_t valor_adc;

#endif
