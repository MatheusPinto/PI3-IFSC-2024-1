# Projeto Integrador 3 - Software

### Microcontrolador

Para o desenvolvimento de firmware do ESP32 foi utilizado o ESP-IDF, framework oficial da Espressif. 

O sistema integra controle de motor de passo, aquisição de dados via ADC (ADS1115) e comunicação Bluetooth SPP (Serial Port Profile). Após receber o comando "ON" via Bluetooth, o dispositivo gira o motor de passo, coleta dados do sensor a cada rotação e envia esses dados formatados via Bluetooth para o app ProxiSense.

## Funcionalidades Principais

1. **Controle do Motor de Passo**:
   - O motor de passo é controlado via GPIO e PWM. Ele gira 200 passos, equivalente a 0.25mm para o micrômetro.
   - A cada 200 passos, o ADC coleta os dados do sensor. São realizadas 12 coletas de dados.

2. **Aquisição de Dados do Sensor**:
   - O sensor ADS1115 é configurado para leitura de tensão em um canal específico (AIN0) com tensão de fundo de escala 4.096V.
   - Os dados do sensor são lidos após cada rotação do motor e convertidos para um formato apropriado.

3. **Comunicação Bluetooth**:
   - O ESP32 atua como um servidor Bluetooth SPP, permitindo a conexão de dispositivos móveis.
   - O sistema responde a comandos recebidos via Bluetooth, como o comando "ON" para iniciar a rotação do motor e a coleta de dados.

## Arquitetura do Código

- **Inicialização do Motor - initMotor**: Configura os pinos GPIO e PWM necessários para controlar o motor de passo.
- **Inicialização do ADS1115 - initADS1115**: Configura o I2C e inicializa o sensor ADS1115 para leitura.
- **Função Principal - app_main**: Configurações iniciais, como o flash de NVS e inicialização do Bluetooth.
- **Função de Callback Bluetooth**: Gerencia eventos Bluetooth, como conexão, desconexão e recebimento de dados.
- **Função de Rotação e Coleta de Dados - rotateMotorAndCollectData**: Gira o motor e coleta dados do sensor em cada rotação, armazenando-os em um vetor.

### App - ProxiSense

Para o desenvolvimento do aplicativo foi utilizado o Flutter, que é um framework de desenvolvimento de aplicações multiplataforma que utiliza a linguagem de programação Dart. Tanto Flutter quanto Dart foram criados pela empresa Google.

Foi utilizado o plugin "flutter_bluetooth_serial" para fazer as integrações nativas do Bluetooth. O plugin só possui suporte para Android.

### Telas do Aplicativo

A primeira tela serve para realizar a configuração do Bluetooth. O celular deve estar pareado ao dispositivo "PROXIMITOR" para iniciar o processo de medida. Clicando em "Configurações", o app abre a tela nativa do Bluetooth para que o usuário possa parear o dispositivo. 

![](https://github.com/maiteluisaa/PI3-IFSC-2024-1/blob/main/gabriel_maite/figures/mainpage.jpeg)

Clicando em "Iniciar" é enviado um comando via Bluetooth para o ESP32 iniciar a medida, caso tenha algum erro ao enviar a mensagem, é apresentada uma mensagem de erro no rodapé da tela. Caso a mensagem tenha sido enviada normalmente, o app fica esperando a resposta do ESP32 com os dados da medida. Quando o app recebe os dados é realizada a comparação da curva formada pelas medidas com a curva do datasheet, caso o erro percentual seja maior que 5%, o sensor precisa ser calibrado.

![](https://github.com/maiteluisaa/PI3-IFSC-2024-1/blob/main/gabriel_maite/figures/resultado.jpeg)

É possível exportar os dados das medidas para um arquivo csv e salvar numa pasta do dispositivo.

