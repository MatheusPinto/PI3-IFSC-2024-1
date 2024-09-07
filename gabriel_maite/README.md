# Projeto Integrador 3 - Sistema de aferição de medição de proximidade para medição de vibração relativa de eixo

O sistema de medição de proximidade é empregado para determinar a distância até a superfície do aço AISI 4140, especialmente para a medição da vibração relativa do eixo. Nesse processo, quanto mais próxima a superfície, menor é a tensão DC gap medida, estabelecendo uma relação inversamente proporcional.

A sonda emite uma onda eletromagnética e é capaz de detectar quaisquer variações no campo gerado por essa onda.

![](https://github.com/maiteluisaa/pi3/blob/main/figures/sensor.jpeg)

O sinal é encaminhado para o demodulador, que realiza a extração da envoltória negativa da curva.

![](https://github.com/maiteluisaa/pi3/blob/main/figures/output.png)

O sistema é composto por uma sonda, um cabo de extensão e um proxímetro. 

![](https://github.com/maiteluisaa/pi3/blob/main/figures/componentes.png)

A curva do proxímetro é fornecida pelo datasheet do fabricante e adaptada às características do aço AISI 4140, geralmente apresenta um fator de escala incremental de aproximadamente 7.87 V/mm.

![](https://github.com/maiteluisaa/pi3/blob/main/figures/curva.png)

Atualmente, a verificação da precisão do sistema de medição é realizada manualmente. Este processo envolve o uso de um micrômetro para ajustar a distância até a superfície do aço AISI 4140, enquanto a tensão de saída do sensor é medida. Cerca de 10 medições são executadas dentro do intervalo estabelecido pelo fornecedor. Os dados coletados são então transferidos para uma planilha do Excel, onde são utilizados para gerar um gráfico e calcular o erro. É importante observar que o fator de escala incremental deve estar dentro de uma tolerância de 5%.

![](https://github.com/maiteluisaa/pi3/blob/main/figures/afericaomanual.png)

No entanto, este método de aferição é intensivo em mão de obra e sujeito a erros humanos. Portanto, este projeto visa automatizar o processo de aferição. Com a capacidade de conter até 20 proxímetros em um tubo gerador, o sistema automatizado eliminará a necessidade de intervenção manual para cada unidade. Isso resultará na redução significativa de erros humanos, otimização do tempo necessário para aferição e aumento da eficiência do trabalho, evitando tarefas repetitivas e monótonas para os trabalhadores.

O projeto consiste em utilizar um micrômetro para ajustar a distância até a superfície do aço AISI 4140, enquanto a tensão de saída do sensor é medida de forma automática pelo microcontrolador. Após a conclusão das medições, os dados são transferidos para um aplicativo de celular através de uma conexão Bluetooth. No aplicativo, a curva é comparada com os padrões de referência e o resultado da análise é imediatamente fornecido ao usuário.
![](https://github.com/maiteluisaa/pi3/blob/main/figures/sistema.png)

### Cronograma de Atividades do Projeto:

|  Atividades                                        | Período    |
| -------------------------------------------------- | ---------- |
| Definição de componentes | 13 a 20 - Março |
| Compra dos componentes | 20 a 31 - Março |
| Integração do micrômetro - RS232 | 01 a 06 - Abril |
| Leitura do sensor | 06 a 17 - Abril |
| Definição do mecanismo do motor/sistema | 24 a 30 - Abril |
| Definição do mecanismo do motor/sistema / Montagem da estrutura | 01 a 08 - Maio |
| Integração do bluetooth / Montagem da estrutura | 08 a 15 - Maio |
| Aplicativo de celular / Montagem da estrutura | 22 a 31 - Maio |
| Aplicativo de celular / Montagem da estrutura | 01 a 05 - Junho |
| Teste do sistema | 05 a 12 - Junho |
| Teste do sistema  | 12 a 19 - Junho |
| Teste do sistema  | 19 a 26 - Junho | 
| Teste do sistema  | 26 a 30 - Junho |
| Apresentação  | 03 - Julho |

### Telas do Aplicativo

A primeira tela serve para realizar a configuração do Bluetooth. O celular deve estar pareado ao dispositivo "ESP32test" para realizar a medição. Clicando em "Configurações", o app abre a tela nativa do Bluetooth para que o usuário possa parear o dispositivo. 

![](https://github.com/maiteluisaa/pi3/blob/main/figures/WhatsApp%20Image%202024-08-14%20at%2018.49.49.jpeg)

Clicando em "Iniciar" é enviado um comando via Bluetooth para o ESP32 iniciar a medida, caso tenha algum erro ao enviar a mensagem, é apresentada uma mensagem de erro no rodapé da tela.

![](https://github.com/maiteluisaa/pi3/blob/main/figures/WhatsApp%20Image%202024-08-14%20at%2018.49.50.jpeg)

Caso a mensagem tenha sido enviada normalmente, o app fica esperando a resposta do ESP32 com os dados da medida. Quando o app recebe os dados é realizada a comparação da curva formada pelas medidas com a curva do datasheet, caso o erro percentual seja maior que 5%, o sensor precisa ser calibrado.

![](https://github.com/maiteluisaa/pi3/blob/main/figures/WhatsApp%20Image%202024-08-14%20at%2019.02.09.jpeg)

É possível exportar os dados das medidas para um arquivo csv e salvar numa pasta do dispositivo.

