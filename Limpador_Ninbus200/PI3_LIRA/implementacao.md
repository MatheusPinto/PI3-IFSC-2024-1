# IMPLEMENTAÇÃO

Nesta etapa iremos de fato iniciar a montagem do projeto de limpesa superficial de piscinas, aplicado a uma maquete, a mesma irá receber tecnologias de sensores e microcontrolador projetados e definidos, mostrado na etapa do design.
Em sua etapa de implementação foram definidos:
* Construção da Maquete física com as devidas entradas e saídas de sensores, de alertas e isolamento da água, para preservar os sensores
* Busca da plataforma de anexação das informações dos sensores, para notificar o usuário por aplicavito.
* Checagem dos parâmetros definidos para melhor leitura dos valores obtidos.
* Motor para movimentação do protótipo definido.



 A implementação foi realizada seguindo a mesma ordem de apresentação desta página. Primeiro a montagem da estrutura da maquete, seguida pela instalação dos sensores, cabos, LED's e outros componentes elétricos e por fim, o teste das conexões e estururas por meio da aplicação de controle.


## MAQUETE
  Para fabricação da maquete para o case do protótipo foi definido de antemao uma caixa de leite com abertura na parte superior. colocando ali os sensores, baterias e demais componentes desejados.
 No entanto, por se tratar de um material com isolamento metálico a medições com ESP8266 dos seus sensores se encontrou inviável, sugerindo assim o case final do protótipo, uma caixa de marmita.

Para a fabricação da maquete, foi utilizado uma maquete modelo sobrado já cortada em MDF, devidamente montada e pintada para posteriormente receber toda a parte eletroeletrônica do projeto, a seguir as medidas da maquete e fotos ilustrativas da montagem:

## SOFTWARE
O Software desejado para implementação do projeto foi o [Blynk IoT](https://blynk.io/) sendo que, Blynk é uma plataforma para IoT que permite a criação de interfaces móveis e web para controlar dispositivos conectados. 
Com uma biblioteca compatível com microcontroladores como o ESP8266, é possível desenvolver aplicativos personalizados para monitoramento e controle remoto de projetos. 
A plataforma oferece ferramentas para criar dashboards intuitivos e conectividade via nuvem, facilitando o desenvolvimento de soluções IoT sem a necessidade de infraestrutura complexa. 
Ideal para automatizar projetos e expandir funcionalidades com rapidez.


## Hardware

No caso de ser um protótipo, o conceito principal foi de não sofrer avarias nas conexões internas por este motivo, os sensores desejados foram:
* MPU6050
* Esp8266
* Sensor NTC Temperatura
* LED's de sinalização
* Buzzer p/ Alerta
* Fios e jumpers
* Baterias
* Motor BLDC 7.5V com caixa de redução.


## CÓDIGO PRINCIPAL
### [Para acessar o código clique aqui.](https://github.com/lirahc/PI3-IFSC-2024-1/blob/main/Limpador_Ninbus200/PI3_LIRA/src/main.cpp).

## FUNCIONAMENTO DO CÓDIGO

Abaixo foi montado um fluxograma para melhor compreensão e explicação do fluxo principal de funcionamento:

<img src="./Imagens/FLUXOGRAMA DE FUNCIONAMENTO DO CÓDIGO.pdf_page_1.jpg" width="1000">

## Esse fluxograma pode ser dividido em quatro partes principais:

# Configuração Inicial:

* Inclui a conexão à rede WiFi e a configuração dos sensores (MPU6050 e DHT11).
* Responsável por garantir que o sistema esteja pronto para funcionar.

# Leitura de Sensores:
* Coleta dos dados de aceleração (MPU6050) e temperatura/umidade (DHT11).
* Inclui o cálculo da magnitude da aceleração para determinar o nível de agitação da água.

# Ações de Controle:
* Controle do buzzer e dos LEDs com base nos dados coletados.
* O buzzer é acionado se a aceleração ultrapassar o limite e os LEDs indicam a direção da aceleração (positiva ou negativa no eixo Z).

# Envio de Dados para Blynk:
* Transmissão das leituras dos sensores (temperatura, umidade, aceleração) para a plataforma Blynk.
* Permite monitoramento remoto e visualização dos dados em tempo real.

Na sua implementação:


**WiFi Setup: **O dispositivo se conecta à rede WiFi usando as credenciais fornecidas.**
Connection Fail: **Se a conexão falhar, o código fica preso tentando conectar novamente.**
Sensor Setup: **Configuração inicial dos sensores MPU6050 (acelerômetro) e DHT11 (umidade/temperatura).**
Sensor Ready: **Após a configuração, os sensores começam a coletar dados.**
Measure Acceleration: **O acelerômetro coleta dados de aceleração da água.**
Alarm/Buzzer: **Se a magnitude da aceleração ultrapassar um valor limite, o buzzer é acionado.**
LED Control: **LEDs são controlados com base na direção da aceleração (eixo Z).**
Data to Blynk: **Os dados coletados (temperatura, umidade e aceleração) são enviados para a plataforma Blynk.**


## 

[Para voltar a página inicial clique aqui.](https://github.com/lirahc/PI3-IFSC-2024-1/blob/main/Limpador_Ninbus200/PI3_LIRA).
