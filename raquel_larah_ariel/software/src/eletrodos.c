#include "eletrodos.h"

/**********************************************
 * Inicializa os pinos do eletrodo (definidos em eletrodos.h)
 ***********************************************/
void eletrodosInit()
{
    ELETRODOS_E_DDRR |= ELETRODOS_E_MASK;
    ELETRODOS_E_PORT &= ~ELETRODOS_E_MASK;
    ELETRODOS_P_DDRR |= ELETRODOS_P_MASK;
    ELETRODOS_P_PORT &= ~ELETRODOS_P_MASK;
    ELETRODOS_N_DDRR |= ELETRODOS_N_MASK;
    ELETRODOS_N_PORT &= ~ELETRODOS_N_MASK;
}

void eletrodosOff()
{
    ELETRODOS_E_PORT &= ~ELETRODOS_E_MASK;
    ELETRODOS_P_PORT &= ~ELETRODOS_P_MASK;
    ELETRODOS_N_PORT &= ~ELETRODOS_N_MASK;
}

void eletrodosPos()
{
    ELETRODOS_E_PORT &= ~ELETRODOS_E_MASK;
    ELETRODOS_P_PORT |= ELETRODOS_P_MASK;
    ELETRODOS_N_PORT &= ~ELETRODOS_N_MASK;
    ELETRODOS_E_PORT |= ELETRODOS_E_MASK;
}

void eletrodosNeg()
{
    ELETRODOS_E_PORT &= ~ELETRODOS_E_MASK;
    ELETRODOS_P_PORT &= ~ELETRODOS_P_MASK;
    ELETRODOS_N_PORT |= ELETRODOS_N_MASK;
    ELETRODOS_E_PORT |= ELETRODOS_E_MASK;
}

void eletrodosInv()
{
    if (ELETRODOS_P_PORT & ELETRODOS_P_MASK)
    {
        eletrodosNeg();
    }
    else if (ELETRODOS_N_PORT & ELETRODOS_N_MASK)
    {
        eletrodosPos();
    }
}

eletrodosStatusType eletrodosStatus(void)
{
    if ((ELETRODOS_E_PORT & ELETRODOS_E_MASK) == 0)
        return ELETRODOS_OFF;
    if (ELETRODOS_P_PORT & ELETRODOS_P_MASK)
        return ELETRODOS_POS;
    if (ELETRODOS_N_PORT & ELETRODOS_N_MASK)
        return ELETRODOS_NEG;
	return ELETRODOS_OFF;
}