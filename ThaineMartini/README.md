# <p align="center"> Comandos, Aquisição e Armazenamento de dados de uma bancada de teste de um sistema de energia híbrido para drone </p>

<p class="text-info recuo" align="justify">
O mercado atual de drones, são raras as opções que possuem autonomia de voo maior que 50 minutos, fator esse que pode dificultar ou mesmo inviabilizar a realização de algumas missões. A marca mais conhecida do mercado (DJI) possui apenas uma opção comercial que promete atuar em uma margem maior de tempo que o limiar mencionado. Essa condição existe por conta das limitações de densidade energética dos armazenadores de energia disponíveis no mercado: baterias. Nesse contexto, uma opção viável para aumentar a autonomia consiste na utilização de conjuntos híbridos que podem ser embarcados na aeronave, utilizando um motor à combustão como parte do sistema de geração de energia elétrica.
</p>
<p class="text-info recuo"align="justify">
Para implementar esse conjunto híbrido no drone, uma solução é importar sistemas pré-fabricados de geração de energia de outros países, como a China. Contudo a hipotética fabricação e nacionalização desse subsistema é uma solução interessante, haja vista o potencial de diminuição de custos, possibilidade de personalização da máquina e ausência dos trâmites de importação.
</p>

<p class="text-info recuo"align="justify">
Para realizar esse projeto, foi desenvolvida uma bancada de testes pelo laboratório de drones do Instituto Federal de Ciência e Tecnologia de Santa Catarina (IFSC). O dispositivo consiste em um motor a combustão interna (MCI) acoplado a um motor BrushLess Direct Current (BLDC), que funciona como gerador de energia, e um retificador trifásico conectado à saída do gerador. Nessa configuração, o acelerador do motor à combustão possui um atuador eletromecânico (servo) que pode ser acionado eletronicamente por um sistema microcontrolado, que deve ajustar automaticamente e em tempo real a posição do acelerador conforme a energia demandada na saída o gerador trifásico (BLDC). Um dos principais desafios dessa simulação é desenvolver um sistema eletrônico de monitoramento e controle que mantenha a tensão constante mesmo com a variação da carga. Com o objetivo de desenvolver esse sistema de controle, é necessário projetar e confeccionar uma placa auxiliar que condiciona os sinais aferidos no sistema para um microcontrolador onde são tratados e armazenados em um log. Essa placa será desenvolvida no projeto integrador 3 e será detalhada a seguir.
</p>

<p class="text-info recuo"align="justify">
Esse projeto integrador deve cumprir os seguintes requisitos, aferir e condicionar os sinais de RPM, temperatura do motor a combustão interna (MCI), corrente e tensão no barramento CC (pós-retificador) do sistema. Deve gerar dois sinais de pulse width modulation (PWM), um para o servo motor que acelera o MCI e outro para o driver que movimenta o BLDC para dar partida no motor. Esta placa também deve possuir dois conversores um de 12V e outro de 5V para a alimentação de payload e periféricos do drone. Assim como tem que gerar um arquivo de dados que contenha todos os sinais que foram monitorados. Na Figura 1, está delineado em formato de diagrama o projeto da PCI.
</p>
<div align="center">
Figura 1 - Diagrama do hardware
<img src="img/diagrama_hardware.jpg">
<figcaption>Fonte: Acervo pessoal</figcaption>
</div>

#### Cronograma idealizado 
<p> 12/03 - Cronograma </p>
<p> 20/03 - Levantamento dos componentes de projeto e pré requisitos </p>
<p> 27/03 - Levantamento dos componentes de projeto e pré requisitos </p>
<p> 03/04 - Esquemático da placa </p>
<p> 10/04 - Esquemático da placa </p>
<p> 17/04 - Layout da placa </p>
<p> 24/04 - Layout da placa </p>
<p> 08/05 - Fabricação da placa, documentação e programação do micro </p>
<p> 15/05 - Fabricação da placa, documentação e programação do micro </p>
<p> 22/05 - 1° TESTE e levantamento de log para análise </p>
<p> 29/05 - Ajustes </p>
<p> 05/06 - 2° TESTE e levantamento de log para análise </p>
<p> 12/06 - Ajustes </p>
<p> 19/06 - Apresentação </p>

## Desenvolvimento

### Componentes
<p> Os componentes foram selecionados de acordo com a disponibilidade no laboratório e com os requisitos o projeto </p>
<p> Servo motor: MG995, a aplicação necessita de um servo robusto e esse componente tem disponível no laboratório </p>
<p> UControlador: F280025C da Texas Instruments, um microcontrolador que aguenta o sistema de controle que será aplicado futuramente </p>
<p> Sensor de corrente: ACS758 150B, o máximo de corrente que o sensor terá que medir é de 80A, o sensor escolhido mede até 150A e tem disponivel no laboratório </p>
<p> Conversor CC/CC 48V - 5V: THL15-4811WL, robusto suficiente para aguentar os picos de tensão (até 60V) e tem disponivel no laboratório </p>
<p> Conversor CC/CC 48V - 12V: EC5SBW-48S12, robusto suficiente para aguentar os picos de tensão (até 60V) e tem disponivel no laboratório </p>
<p> Sensor de temperatura: NTC 33950 100k, o sensor deve medir pelo menos de 10 C até 200 C, o sensor escolhido mede de -4 a 270 C </p>
<p> Amplificador Opercional: TLV 9044 DIR, escolhido pela disponibilidade no laboratório </p>
<p> Regulador de Tensão: LM 1117 </p>

### Projeto
<p align="justify"> O projeto foi realizado de acordo com diagrama de blocos demonstrado anteriormente</p>

#### Leitura da corrente

<p align="justify">O circuito utilizado para a leitura da corrento envolve o sensor (ACS 758B), o condicionamento de sinal e um filtro ativo passa-baixa (Sallen-Key). Para a adequar os niveis de tensão foi utilizado um circuito subtrator, descrito abaixo </p>

<picture>
 <img alt="circuito para aferir a corrente" src="img/amplificador-subtrator-com-ampop.png">
</picture>

<p align="justify">Para a tensão V1 maxima, levamos em consideração que quando a corrente é nula a tensão de saída do ACS vai ser Vcc/2 = 1.65.</p>
<p align="justify">Para a tensão V1 minima, levamos em consideração que quando a corrente é maxima a tensão de saída do ACS vai ser (Vcc/2) + (Corrente maxima * sensibilidade do sensor), a sensibilidade do sensor pelo datasheet é 13*10^-3 e a corrente  maxima prevista é 100 A (com uma certa sobra por precaução), com isso temos V2 = 2.95 </p>
<p align="justify">Para V2 é utilizado um divisor resistivo para conseguir a tensão de 1.5V (R1 = 1.2k ohm e R2=1k ohm).  </p>

<picture>
 <img alt="circuito para aferir a corrente" src="img/corrente.png">
</picture>

#### Leitura da tensão 
<p align="justify"> Para a leitura da Tensão foi usado um divisor resistivo de tensão juntamente com um filtro Sallen-Key de segunda ordem (passa-baixa) para a diminuição de ruido na leitura do sinal. Para calcular o divisor resistivo foi assumindo que R1 = 56560 ohm (dividido em dois resistores um de 56k ohm e outro e 560 ohm), R2=3k3 ohm e com uma tensão maxima de 60V. Para o calculo do filtro Sallen-Key, foi previsto pelos professores do laboratori de dornes uma frequência de corte de 723 Hz para o filtro, para a definição dos capacitores e resistores foi utilizada a formula fc = 1/(2*pi*sqrt(R1*C1*R2*C2)) onde ao definir C1 e C2 com 100 nF, temos R1 e R2 de 2k2 ohm. O esquemático é mostrado na figura abaixo. </p>

<picture>
 <img alt="circuito para aferir a tensão" src="img/tensão.png">
</picture>

### Esquemático 
<p align="justify">Através do software KiCad e com os requisitos levantados anteriormente foi realizado a implementação do esquemático (pdf com o esquemático completo na pasta do projeto), que é composto por 5 Blocos principais: </p>
<p> - Alimentação: O bloco de alimentação é formado pela conexão da Bateria de 12S (aproximadamente 48V) e a conexão do Gerador Elétrico</p>
<p> - Potência: Possui dois conversores THL15-4811WL (para 5V) e o EC5SBW-48S12 (para 12V), com o circuito sugerido na folha de dados</p>
<p> - Conexões: São as conexões que a placa possui com o meio esterno, primeramente o 4 conectores XT60 que são usados para a conectar a carga (para testes inicias uma carga resistiva), 3 conectores JST-XH de duas vias, um para a alimentação da bomba que auxilia no resfriamento o motor, outro para aferir a temperatura e por último para a ignição do motor, 6 conectores JST-XH de 3 vias sendo, para o esc que vai acionar o BLDC para dar a partida no motor a combustão, para o servo motor que auxilia no controle, um que recebe um pulso da controladora de voo para o acionamento remoto do motor, e dois reservas caso venha a precisar no futuro, alem desses conectores temos nesse bloco um optacoplador para proteção do circuito com o sinal que vem do tacomêtro, um jumper para a alimentação do F280025C caso precise debugar o circuito no computador, um modulo para leitura do cartão SD e dois leds para informações. Para a comunicação com a controladora de voo foi prevista duas conexões, uma por JST-XH  e uma por JST-GH, para conseguir utilizar os dois tipos de controlaora disponiveis no laboratório (cube orange e pixhalk)</p>
<p> - Condicionamento do Sinal: Contendo dois sinais que precisam ser ajustados para a leitura no F280025C a tensão que passa por um divisor resistivo para diminuir a tensão, um buffer e um filtro Sallen-Key e a corrente que após passar por um sensor de efeito hall (ACS758) passa por um buffer, um circuito somador para amplificar a tensão fornecida pelo sensor e um filtro sallen-key, o detalhemento desses dois circuitos está fornecido na folha de calculos.</p>
<p></p>

### Layout

<p>O layout seguiu os seguintes requisitos:</p>
  <p>- Deixar todos os componentes na parte superior da placa assim como os pontos de medição de tensão</p>
  <p>- A parte de potência separada e com menor interatividade com o resto da placa</p>
  <p>- Conectores na borda da placa </p>
  <p>- Malha de potência no barramento (no bloco de alimentação) </p>
  <p align="justify">Os requisitos foram pensados para evitar ruídos indesejados, evitar que a placa sobreaqueça, melhor conexão dos perifericos e para que acaso haja algum problema no futuro seja fácil de encontrar o que está gerando.</p>

### Fabricação 

<p align="justify">Para a montagem da placa, foi utilizado o laboratório de drones, onde possui a infraestrutura e os componentes. Após a montegem foi realizado os testes paralelamente. Primeiramente a montagem dos conversore, depois o circuitos de tensão, o circuito de corrente e por último os circuitos perifericos</p>
<picture>
 <img alt="placa montada" src="img/plc.jpeg">
</picture>

### Testes
<p align="justify">Para os conversores foi verificado os Leds correspondente a cada tensão (12V e 5V) e realizado a leitura em cada ponto de alimentação através de um multimeto, a placa respondeu conforme o esperado, para o teste de tensão e corrente, foi seguido o seguinte procedimento:</p>
 <p>- Conetar a entrada da placa em uma bateria de 12s (+- 48V) </p>
 <p>- Conectar a saída da placa (XT60) em uma carga resistiva variavél</p>
 <p>- Aferir com um multimetro os pontos: corrente na carga,	tensão na bateria, tensão na saída do	ACS, tensão no iBAT e tensão no	VBUS</p>
 <p>- Aumentar a corrente (associando mais resistores) e reproduzir o procedimento</p>

<p align="justify">Com isso foi possível preencher a tabela apresentada a seguir</p>

 <img alt="tabela dos resultados" src="img/comparacao.png">

 <p align="justify">Através dessa tabela, o objetivo da placa é concluido. Apesar de ter um erro, ele é desconsideravél já que em correntes mais altas, que é onde a placa atua, o erro se matém em 1%</p>
 
### Programação

<p align="justify">Na parte de programação, está sendo desenvolvido um script que faz a leitura do Vbus e do ibat e armazena eles em um arquivo de log. O que se obteve até agora foi apenas efetuar o print na tela dessas tensões lidas, o que já é eficiente para o seu objetivo final.</p>
