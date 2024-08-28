#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include <stdint.h>

void updateTargetTemp(VAR_TEMPERATURA_T val);
void updateTempo(VAR_TEMPO_T val);
void updateIntervalo(VAR_INTERVALO_T val);
void updateVelocidade(VAR_VELOCIDADE_T val);
void updateError(uint8_t val);
void updateTemperatura(uint8_t val);
void updateCorrente(uint16_t val);

/******************************************TELA DE SELECAO */
typedef enum
{
    SO_NEXT,
    SO_TTEMP,
    SO_TEMPO,
    SO_INT,
    SO_VEL
} selecaoOpts;
void telaSelecao(selecaoOpts sel);
/****************************************************TELA DE EDICAO */
typedef enum
{
    ED_TTEMP = 0,
    ED_TEMPO,
    ED_INT,
    ED_VEL,
    ED_TERR
} edicaoOpts;
void telaEdicao(edicaoOpts opts);


void telaAquecendo(void);

void telaFuncionamento(void);
#endif
