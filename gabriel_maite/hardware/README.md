# SISTEMA DE AFERIÇÃO DE PROXIMITOR

O maior desafio em aplicações que utilizam o proximitor é o sinal de saída com um range de 0 a -24 V, o que dificulta a integração com sistemas digitais. Neste projeto, o conversor analógico-digital de 16 bits ADS1115IDGSR (ADC) é responsável por comunicar a tensão ao microcontrolador ESP32 que, por sua vez, é encarregado de realizar a aferição do sistema de forma automática. Como ambos os componentes operam em faixas de tensão positivas, é necessário incluir um circuito condicionador de sinal para adequar a tensão de saída do proximitor de acordo com a faixa de operação do ADC.

## Condicionador de Sinal

O condicionador de sinal consiste em três blocos: divisor de tensão, buffer e inversor de tensão. O divisor de tensão é responsável por ajustar a tensão do sinal de saída do proximitor para uma faixa de operação do amplificador operacional (ampop) (±15 V). Em seguida, o buffer atua para isolar o sinal, garantindo que as medições não sejam influenciadas por outros componentes do circuito. Por fim, o inversor de tensão converte o sinal para a faixa positiva, compatível com a entrada A0 do ADC, permitindo que o ESP32 execute as medições de forma precisa e eficiente.

    O ADC está configurado para medir tensões de 0 a 24 V com a entrada A0. No módulo do conversor tem um divisor resistivo implementado de fábrica que pode ser habilidado através de um jumper de solda na placa. O divisor resistivo divide a tensão de entrada por 5,84, dessa forma, a tensão de 24 V é transformada em 4.1 V.

## CIRCUITO COMPLETO

O circuito consta com três fontes de 24 V, duas operam como fontes simétricas com auxílio reguladores de tensão de ± 15 V (LM7815 E LM7915) para a alimentação do ampop, e um regulador de tensão de 5 V (LM7805) para a alimentação do ESP32. O driver do motor (DRV8825) depende de uma referência isolada para o motor, sendo necessário o uso da terceira fonte.

## ESQUEMÁTICO DO CIRCUITO COMPLETO
![image][schematic]

## PCB
![image][pcb]

## PCB 3D
![image][pcb3d]

[schematic]: figures/schematic.png
[pcb]: figures/PCB.png
[pcb3d]: figures/PCB3D.png