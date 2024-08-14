#ifndef VARIABLES_H_INCLUDED
#define VARIABLES_H_INCLUDED

#include <stdio.h>

/******************************************************************************/
/*******************************TEMPERATURA ***********************************/
#define VAR_TEMPERATURA_DEFAULT 60
#define VAR_TEMPERATURA_MIN 60
#define VAR_TEMPERATURA_MAX 60
#define VAR_TEMPERATURA_STEP 20
#define VAR_TEMPERATURA_STR_LENGTH 2
#define VAR_TEMPERATURA_T uint8_t
#define VAR_TEMPERATURA_TMAX UINT8_MAX
#define VAR_TEMPERATURA_TMIN 0

void var_temperatura_set(VAR_TEMPERATURA_T v);
void var_temperatura_inc(void);
void var_temperatura_dec(void);
/******************************************************************************/
/*******************************TEMPO******************************************/
#define VAR_TEMPO_DEFAULT 120
#define VAR_TEMPO_MIN 30
#define VAR_TEMPO_MAX 120
#define VAR_TEMPO_STEP 10
#define VAR_TEMPO_STR_LENGTH 4 // "h:mm"
#define VAR_TEMPO_T uint8_t
#define VAR_TEMPO_TMAX UINT8_MAX
#define VAR_TEMPO_TMIN 0

void var_tempo_set(VAR_TEMPO_T v);
void var_tempo_inc(void);
void var_tempo_dec(void);
/******************************************************************************/
/*******************************INTERVALO**************************************/
#define VAR_INTERVALO_DEFAULT 4
#define VAR_INTERVALO_MIN 0
#define VAR_INTERVALO_MAX 9
#define VAR_INTERVALO_STEP 1
#define VAR_INTERVALO_STR_LENGTH 1
#define VAR_INTERVALO_T uint8_t
#define VAR_INTERVALO_TMAX UINT8_MAX
#define VAR_INTERVALO_TMIN 0

void var_intervalo_set(VAR_INTERVALO_T v);
void var_intervalo_inc(void);
void var_intervalo_dec(void);
/******************************************************************************/
/*******************************VELOCIDADE*************************************/
#define VAR_VELOCIDADE_DEFAULT 50
#define VAR_VELOCIDADE_MIN 0
#define VAR_VELOCIDADE_MAX 100
#define VAR_VELOCIDADE_STEP 10
#define VAR_VELOCIDADE_STR_LENGTH 2
#define VAR_VELOCIDADE_T uint8_t
#define VAR_VELOCIDADE_TMAX UINT8_MAX
#define VAR_VELOCIDADE_TMIN 0

void var_velocidade_set(VAR_VELOCIDADE_T v);
void var_velocidade_inc(void);
void var_velocidade_dec(void);
/******************************************************************************/
/*******************************MOTOR******************************************/
#define VAR_MOTOR_ON 0
#define VAR_MOTOR_OFF 1
#define VAR_MOTOR_DEFAULT VAR_MOTOR_OFF
#define VAR_MOTOR_T uint8_t

void var_motor_on(void);
void var_motor_off(void);
/******************************************************************************/
/*******************************ELETRODO***************************************/
#define VAR_ELETRODO_OFF 0
#define VAR_ELETRODO_POS 1
#define VAR_ELETRODO_NEG 2
#define VAR_ELETRODO_DEFAULT VAR_ELETRODO_OFF
#define VAR_ELETRODO_T uint8_t

void var_eletrodo_pos(void);
void var_eletrodo_neg(void);
void var_eletrodo_off(void);
/******************************************************************************/
/*******************************AQUECE*****************************************/
#define VAR_AQUECE_ON 0
#define VAR_AQUECE_OFF 1
#define VAR_AQUECE_DEFAULT VAR_AQUECE_OFF
#define VAR_AQUECE_T uint8_t

void var_aquece_on(void);
void var_aquece_off(void);
/******************************************************************************/

typedef struct
{
    struct
    {
        VAR_TEMPERATURA_T temperatura;
        VAR_TEMPO_T tempo;
        VAR_INTERVALO_T intervalo;
        VAR_VELOCIDADE_T velocidade;
        VAR_MOTOR_T motor : 1;
        VAR_ELETRODO_T eletrodo : 2;
        VAR_AQUECE_T aquece : 1;
    } values;
    struct
    {
        char temperatura[VAR_TEMPERATURA_STR_LENGTH + 1];
        char tempo[VAR_TEMPO_STR_LENGTH + 1];
        char intervalo[VAR_INTERVALO_STR_LENGTH + 1];
        char velocidade[VAR_VELOCIDADE_STR_LENGTH + 1];
    } str;
} variablesType;

extern const variablesType *const variables;

void var_Init(void);

#endif
