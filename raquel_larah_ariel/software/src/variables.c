#include <stdio.h>

#include "variables.h"

#define STR_IMPL_(...) #__VA_ARGS__
#define STR(...) STR_IMPL_(__VA_ARGS__)

static variablesType vs = {
    .values = {
        .temperatura = VAR_TEMPERATURA_DEFAULT,
        .tempo = VAR_TEMPO_DEFAULT,
        .intervalo = VAR_INTERVALO_DEFAULT,
        .velocidade = VAR_VELOCIDADE_DEFAULT,
        .motor = VAR_MOTOR_DEFAULT,
        .eletrodo = VAR_ELETRODO_DEFAULT,
        .aquece = VAR_AQUECE_DEFAULT,
    },
    .str = {
        .temperatura = {0},
        .tempo = {0},
        .intervalo = {0},
        .velocidade = {0},
    },
};

const variablesType *const variables = &vs;

void var_Init(void)
{
    var_temperatura_set(VAR_TEMPERATURA_DEFAULT);
    var_tempo_set(VAR_TEMPO_DEFAULT);
    var_intervalo_set(VAR_INTERVALO_DEFAULT);
    var_velocidade_set(VAR_VELOCIDADE_DEFAULT);

#if (VAR_MOTOR_DEFAULT == VAR_MOTOR_OFF)
    var_motor_off();
#elif (VAR_MOTOR_DEFAULT == VAR_MOTOR_ON)
    var_motor_on();
#else
#error(unknown DEFAULT)
#endif

#if (VAR_ELETRODO_DEFAULT == VAR_ELETRODO_OFF)
    var_eletrodo_off();
#elif (VAR_ELETRODO_DEFAULT == VAR_ELETRODO_POS)
    var_eletrodo_pos();
#elif (VAR_ELETRODO_DEFAULT == VAR_ELETRODO_NEG)
    var_eletrodo_neg();
#else
#error(unknown DEFAULT)
#endif

#if (VAR_AQUECE_DEFAULT == VAR_AQUECE_OFF)
    var_aquece_off();
#elif (VAR_AQUECE_DEFAULT == VAR_AQUECE_ON)
    var_aquece_on();
#else
#error(unknown DEFAULT)
#endif
}

/******************************************************************************/
/*******************************TEMPERATURA************************************/
#define TEMPERATURA_STR_FMT "%0" STR(VAR_TEMPERATURA_STR_LENGTH) "d"
void var_temperatura_set(VAR_TEMPERATURA_T v)
{
    if (v < VAR_TEMPERATURA_MIN)
        vs.values.temperatura = 0;
    else if (v > VAR_TEMPERATURA_MAX)
        vs.values.temperatura = VAR_TEMPERATURA_MAX;
    else
        vs.values.temperatura = v;
    snprintf(vs.str.temperatura, VAR_TEMPERATURA_STR_LENGTH + 1, TEMPERATURA_STR_FMT, vs.values.temperatura);
}
void var_temperatura_inc(void)
{
    if (vs.values.temperatura < (VAR_TEMPERATURA_TMAX - VAR_TEMPERATURA_STEP))
    {
		if (vs.values.temperatura == 0) 
		{
			var_temperatura_set(VAR_TEMPERATURA_MIN);
		}
		else 
		{
			var_temperatura_set(vs.values.temperatura + VAR_TEMPERATURA_STEP);
		}
    }
    else
    {
        var_temperatura_set(VAR_TEMPERATURA_TMAX);
    }
}
void var_temperatura_dec(void)
{
    if (vs.values.temperatura > (VAR_TEMPERATURA_TMIN + VAR_TEMPERATURA_STEP))
    {
        var_temperatura_set(vs.values.temperatura - VAR_TEMPERATURA_STEP);
    }
    else
    {
        var_temperatura_set(VAR_TEMPERATURA_TMIN);
    }
}
/******************************************************************************/
/*******************************TEMPO******************************************/
void var_tempo_set(VAR_TEMPO_T v)
{
    if (v < VAR_TEMPO_MIN)
        vs.values.tempo = VAR_TEMPO_MIN;
    else if (v > VAR_TEMPO_MAX)
        vs.values.tempo = VAR_TEMPO_MAX;
    else
        vs.values.tempo = v;
    // TODO: Revisitar essa lógica aqui, talves a exibição nesse formato não seja a melhor
    snprintf(vs.str.tempo, VAR_TEMPO_STR_LENGTH + 1, "%1d:%02d", vs.values.tempo / 60, vs.values.tempo % 60);
}
void var_tempo_inc(void)
{
    if (vs.values.tempo < (VAR_TEMPO_TMAX - VAR_TEMPO_STEP))
    {
        var_tempo_set(vs.values.tempo + VAR_TEMPO_STEP);
    }
    else
    {
        var_tempo_set(VAR_TEMPO_TMAX);
    }
}
void var_tempo_dec(void)
{
    if (vs.values.tempo > (VAR_TEMPO_TMIN + VAR_TEMPO_STEP))
    {
        var_tempo_set(vs.values.tempo - VAR_TEMPO_STEP);
    }
    else
    {
        var_tempo_set(VAR_TEMPO_TMIN);
    }
}
/******************************************************************************/
/*******************************INTERVALO**************************************/
#define INTERVALO_STR_FMT "%0" STR(VAR_INTERVALO_STR_LENGTH) "d"
void var_intervalo_set(VAR_INTERVALO_T v)
{
    if (v < VAR_INTERVALO_MIN)
        vs.values.intervalo = VAR_INTERVALO_MIN;
    else if (v > VAR_INTERVALO_MAX)
        vs.values.intervalo = VAR_INTERVALO_MAX;
    else
        vs.values.intervalo = v;
    snprintf(vs.str.intervalo, VAR_INTERVALO_STR_LENGTH + 1, INTERVALO_STR_FMT, vs.values.intervalo);
}
void var_intervalo_inc(void)
{
    if (vs.values.intervalo < (VAR_INTERVALO_TMAX - VAR_INTERVALO_STEP))
    {
        var_intervalo_set(vs.values.intervalo + VAR_INTERVALO_STEP);
    }
    else
    {
        var_intervalo_set(VAR_INTERVALO_TMAX);
    }
}
void var_intervalo_dec(void)
{
    if (vs.values.intervalo > (VAR_INTERVALO_TMIN + VAR_INTERVALO_STEP))
    {
        var_intervalo_set(vs.values.intervalo - VAR_INTERVALO_STEP);
    }
    else
    {
        var_intervalo_set(VAR_INTERVALO_TMIN);
    }
}

/******************************************************************************/
/*******************************VELOCIDADE*************************************/
#define VELOCIDADE_STR_FMT "%" STR(VAR_VELOCIDADE_STR_LENGTH) "d"
void var_velocidade_set(VAR_VELOCIDADE_T v)
{
    if (v < VAR_VELOCIDADE_MIN)
        vs.values.velocidade = VAR_VELOCIDADE_MIN;
    else if (v > VAR_VELOCIDADE_MAX)
        vs.values.velocidade = VAR_VELOCIDADE_MAX;
    else
        vs.values.velocidade = v;
    snprintf(vs.str.velocidade, VAR_VELOCIDADE_STR_LENGTH + 1, VELOCIDADE_STR_FMT, vs.values.velocidade);
}
void var_velocidade_inc(void)
{
    if (vs.values.velocidade < (VAR_VELOCIDADE_TMAX - VAR_VELOCIDADE_STEP))
    {
        var_velocidade_set(vs.values.velocidade + VAR_VELOCIDADE_STEP);
    }
    else
    {
        var_velocidade_set(VAR_VELOCIDADE_TMAX);
    }
}
void var_velocidade_dec(void)
{
    if (vs.values.velocidade > (VAR_VELOCIDADE_TMIN + VAR_VELOCIDADE_STEP))
    {
        var_velocidade_set(vs.values.velocidade - VAR_VELOCIDADE_STEP);
    }
    else
    {
        var_velocidade_set(VAR_VELOCIDADE_TMIN);
    }
}

/******************************************************************************/
/*******************************MOTOR******************************************/
void var_motor_on(void)
{
    vs.values.motor = VAR_MOTOR_ON;
}
void var_motor_off(void)
{
    vs.values.motor = VAR_MOTOR_OFF;
}
/******************************************************************************/
/*******************************ELETRODO***************************************/
void var_eletrodo_pos(void)
{
    vs.values.eletrodo = VAR_ELETRODO_POS;
}
void var_eletrodo_neg(void)
{
    vs.values.eletrodo = VAR_ELETRODO_NEG;
}
void var_eletrodo_off(void)
{
    vs.values.eletrodo = VAR_ELETRODO_OFF;
}
/******************************************************************************/
/*******************************AQUECE*****************************************/
void var_aquece_on(void)
{
    vs.values.aquece = VAR_AQUECE_ON;
}
void var_aquece_off(void)
{
    vs.values.aquece = VAR_AQUECE_OFF;
}
/******************************************************************************/