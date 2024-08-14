#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include <stdint.h>

uint8_t tela_selecao(uint8_t sel);
uint8_t tela_edicao(uint8_t sel);
uint8_t tela_erro(const char *msg, uint8_t codigo);
void tela_aquecendo();

#endif
