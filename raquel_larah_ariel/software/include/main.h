#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED
#include <stdint.h>

#define FSM_BASE_TIME_MS 1UL

/*******************************TEMPERATURA ***********************************/
#define VAR_TEMPERATURA_DEFAULT 60
#define VAR_TEMPERATURA_MIN 50
#define VAR_TEMPERATURA_MAX 99
#define VAR_TEMPERATURA_STEP 1
#define VAR_TEMPERATURA_STR_LENGTH 2
#define VAR_TEMPERATURA_T uint8_t
#define VAR_TEMPERATURA_TMAX UINT8_MAX
#define VAR_TEMPERATURA_TMIN 0
/*******************************TEMPO******************************************/
#define VAR_TEMPO_DEFAULT 120
#define VAR_TEMPO_MIN 30
#define VAR_TEMPO_MAX 120
#define VAR_TEMPO_STEP 10
#define VAR_TEMPO_STR_LENGTH 4 // "h:mm"
#define VAR_TEMPO_T uint8_t
#define VAR_TEMPO_TMAX UINT8_MAX
#define VAR_TEMPO_TMIN 0
/*******************************INTERVALO**************************************/
#define VAR_INTERVALO_DEFAULT 4
#define VAR_INTERVALO_MIN 0
#define VAR_INTERVALO_MAX 9
#define VAR_INTERVALO_STEP 1
#define VAR_INTERVALO_STR_LENGTH 1
#define VAR_INTERVALO_T uint8_t
#define VAR_INTERVALO_TMAX UINT8_MAX
#define VAR_INTERVALO_TMIN 0
/*******************************VELOCIDADE*************************************/
#define VAR_VELOCIDADE_DEFAULT 50
#define VAR_VELOCIDADE_MIN 0
#define VAR_VELOCIDADE_MAX 100
#define VAR_VELOCIDADE_STEP 10
#define VAR_VELOCIDADE_STR_LENGTH 3
#define VAR_VELOCIDADE_T uint8_t
#define VAR_VELOCIDADE_TMAX UINT8_MAX
#define VAR_VELOCIDADE_TMIN 0
/*******************************MOTOR******************************************/
#define VAR_MOTOR_ON 0
#define VAR_MOTOR_OFF 1
#define VAR_MOTOR_DEFAULT VAR_MOTOR_OFF
#define VAR_MOTOR_T uint8_t
/*******************************ELETRODO***************************************/
#define VAR_ELETRODO_OFF 0
#define VAR_ELETRODO_POS 1
#define VAR_ELETRODO_NEG 2
#define VAR_ELETRODO_DEFAULT VAR_ELETRODO_OFF
#define VAR_ELETRODO_T uint8_t
/*******************************AQUECE*****************************************/
#define VAR_AQUECE_ON 0
#define VAR_AQUECE_OFF 1
#define VAR_AQUECE_DEFAULT VAR_AQUECE_OFF
#define VAR_AQUECE_T uint8_t
/*******************************CORRENTE***************************************/
#define VAR_CORRENTE_ADC_MIN 0
#define VAR_CORRENTE_ADC_MAX ((1 << 10) - 1)
#define VAR_CORRENTE_OUT_MIN -1200
#define VAR_CORRENTE_OUT_MAX 1200
#define VAR_CORRENTE_OUT_THRESHOLD 1000
#define VAR_CORRENTE_ADC_THRESHOLD (uint16_t)(                              \
    ((int32_t)VAR_CORRENTE_OUT_THRESHOLD - (int32_t)VAR_CORRENTE_OUT_MIN) * \
        ((int32_t)VAR_CORRENTE_ADC_MAX - (int32_t)VAR_CORRENTE_ADC_MIN) /   \
        ((int32_t)VAR_CORRENTE_OUT_MAX - (int32_t)VAR_CORRENTE_OUT_MIN) +   \
    (int32_t)VAR_CORRENTE_ADC_MIN)
/******************************************************************************/

#define PIN_AQUECE_PORT PORTB
#define PIN_AQUECE_DDRR DDRB
#define PIN_AQUECE_PINR PINB
#define PIN_AQUECE_MASK (1 << PINB7)

#define PIN_MOTOR_PORT PORTB
#define PIN_MOTOR_DDRR DDRB
#define PIN_MOTOR_PINR PINB
#define PIN_MOTOR_MASK (1 << PINB7)

#define PIN_ELETRODOS_E_PORT PORTB
#define PIN_ELETRODOS_E_DDRR DDRB
#define PIN_ELETRODOS_E_PINR PINB
#define PIN_ELETRODOS_E_MASK (1 << PINB7)

#define PIN_ELETRODOS_P_PORT PORTB
#define PIN_ELETRODOS_P_DDRR DDRB
#define PIN_ELETRODOS_P_PINR PINB
#define PIN_ELETRODOS_P_MASK (1 << PINB7)

#define PIN_ELETRODOS_N_PORT PORTB
#define PIN_ELETRODOS_N_DDRR DDRB
#define PIN_ELETRODOS_N_PINR PINB
#define PIN_ELETRODOS_N_MASK (1 << PINB7)

#define PIN_CORRENTE_PORT PORTC
#define PIN_CORRENTE_DDRR DDRC
#define PIN_CORRENTE_PINR PINC
#define PIN_CORRENTE_MASK (1 << PINC0)

#endif
