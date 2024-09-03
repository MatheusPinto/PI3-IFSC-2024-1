<table align="center"><tr><td align="center" width="9999"><br>
<img src="image/logo.png" align="center" width="150" alt="Logo IFSC">

# Unidade Curricular:  Projeto Integrador 3 

<b>Instituto Federal de Educação, Ciência e Tecnologia de Santa Catarina<br>
Campus Florianópolis<br></b>
**Professor:**  Robinson Pizzio e Matheus Leitzke Pinto<br>
**Alunos:**  Alexsander Vieira e Jonathan Chrysostomo Cabral Bonette<br>
</td></tr></table>

* [Introdução](#introdução)
* [Componentes utilizados](#componentes-utilizados)
* [Desenvolvimento](#desenvolvimento)

## 1. Introdução

O projeto do Smart Mirror visa desenvolver um espelho inteligente capaz de exibir informações úteis diretamente na superfície refletora enquanto o usuário realiza suas atividades diárias. A inovação deste dispositivo está na integração de tecnologia avançada com um objeto comum, transformando o espelho em uma interface interativa que proporciona acesso rápido a uma variedade de serviços e informações.

O principal objetivo do Smart Mirror é fornecer informações relevantes de forma prática e imediata, integrando-as ao momento em que o usuário se prepara para o dia. Com uma interface intuitiva, o espelho pode exibir dados como a previsão do tempo, notícias, agenda do dia, notificações de redes sociais, status de dispositivos inteligentes na casa, entre outros.

## 2. Requisitos

**Requisitos Funcionais:** O que o Smart Mirror deve ser capaz de fazer interagir com o usuário de forma a permitir a transição entre a operação como um espelho comum(apenas refletir o usuário) e como um dispositivo smart (apresentar funcionalidades de conectividade). O 
Exemplos de aplica
~~Requisitos Não Funcionais: Inclua aspectos como desempenho, confiabilidade, usabilidade, e segurança.~~
**Requisitos de Software e Hardware:** O sistema devera ser implementado utilizando como hardware um Raspberry Pi 4 Model B com Raspberry Pi OS, onde será desenvolvido a aplicação do smart mirror e a integração de módulos que tragam funcionalidades úteis ao dia a dia do usuário.

## Lista de Componentes

*   Raspberry Pi 4 Model B 
	 * Conectividade Wi-Fi Integrada
	 * Saída de vídeo HDMI integrada
	 * Capacidades de Comunicação IO - leitura de sensores
     * OS ???
     * Custo aproximado: R$250
     
     ### Raspberry Pi 4 Model B
![Raspberry Pi 4 Model B](https://www.pi-shop.ch/media/catalog/product/cache/1/image/650x/040ec09b1e35df139433887a97daa66f/t/h/thumbnail_10__2.jpg )
 *   Monitor
	 * Resolução: xxxXxxx
	 * Entrada de vídeo HDMI integrada
	 * Disponibilidade, reduzir custo
     * Custo aproximado: R$xxxxxxx
     
     ### Monitor
![Raspberry Pi 4 Model B](______)
     
 *   Monitor
	 * Resolução: xxxXxxx
	 * Entrada de vídeo HDMI integrada
	 * Disponibilidade
     * Custo aproximado: R$xxxxxxx
     
     ### Monitor
![Monitor](______)

 *   Moldura de Madeira
	 * Fixar e dar estrutura ao conjunto.
     * Custo aproximado: R$xxxxxxx
     
     ### Moldura de madeira
![Moldura de madeira](______)

 *   Vidro
	 * Dimensão
	 * Espessura
     * Custo aproximado: R$xxxxxxx
     
     ### Vidro
![Vidro](______)

 *   Película espelhada
	 * xxxxxxxx
     * Custo aproximado: R$xxxxxxx
     
     ### Película espelhada
![Monitor](______)
 *   Sensor de gestos
	 * Modelo: xxxXxxx
	 * Capacidade de interagir com sistema
     * Custo aproximado: R$xxxxxxx
     
     ### Sensor de gestos
![Sensor de gestos](______)

 *   ~~Moldura touch screem~~
	 * ~~Facilidade de interagir com sistema~~
     * ~~Custo aproximado:~~ R$xxxxxxx
     
     ### ~~Moldura touch screem~~
![Raspberry Pi 4 Model B](______)

## Montagem do Hardware

Construção da Moldura: ~~Descreva o processo de construção da moldura de madeira, incluindo medições e montagem~~.

### Moldura lado frente
![Moldura lado A](Moldura_v1_A.jpeg )

### Moldura lado traseira
![Moldura lado B](Moldura_v1_B.jpeg )

Instalação do Monitor e Vidro: ~~Detalhe como o monitor e o vidro com película reflexiva foram instalados na moldura.~~

### Vidro Película 1
![Vidro Película 1](Vidro_pelicula_1.jpeg )

### Vidro Película 2
![Vidro Película 2](Vidro_pelicula_2.jpeg )

Instalação do Sensor de Movimento: ~~Explique onde e como o sensor de movimento foi instalado para detectar gestos.~~

### Sensor de gestos
![Sensor de gestos](Sensor_gestos.jpeg )

Conexões e Configurações Finais: ~~Descreva como o hardware foi conectado e montado, incluindo a ligação ao Raspberry Pi.~~

### Montagem final
![Montagem final](Montagem_final.jpeg )

## Configuração do Software

Instalação do Sistema Operacional: ~~Explicação passo a passo da instalação do sistema operacional no Raspberry Pi.~~

Configuração do Smart Mirror: ~~Detalhe como o software  foi instalado e configurado.~~

Instalação e customização dos Módulos:

Modulo agenda.  
Etapas:

1. 1
2. 2

 ```C

 ```

Modulo calendário.  
Etapas:

1. 1
2. 2

 ```C

 ```
 
Modulo previsão de tempo.  
Etapas:

1. 1
2. 2

 ```C

 ```

Modulo Cumprimentos/frase motivacionais.  
Etapas:

1. 1
2. 2

 ```C

 ```

Modulo Formula 1.  
Etapas:

1. 1
2. 2

 ```C

 ```

Modulo Rádio.
Etapas:

1. 1
2. 2

 ```C

 ```
 
Modulo Feed de noticias.
Etapas:

1. 1
2. 2

 ```C

 ```

Modulo Câmeras segurança.  
Etapas:

1. 1
2. 2

 ```C

 ```
 
Modulo Informações de sistema.  
Etapas:

1. 1
2. 2

 ```C

 ```
 
Modulo Horários de ônibus.  
Etapas:

1. 1
2. 2

 ```C

 ```
Modulo Spotify.  
Etapas:

1. 1
2. 2

 ```C

 ```
## Funcionalidades Implementadas

O Prototipo a ser apresentado foi organizado de forma a apresentar 3 telas com exemplos de aplicação para o usuário.

 1. Tela 1:  Nesta tela apresentamos um exemplo de integração entre os módulos de cumprimentos, calendário e previsão do tempo. Demonstrando interface proporcione acesso rápido a informações do dia a dia do usuário ao se preparar para sair.

### Tela 1
![Tela 1](Tela_1.jpeg )

 2. Tela 2:  Nesta tela apresentamos um exemplo de integração entre os módulos de formula 1, rádio e noticias. Demonstrando uma aplicação mais  voltada a entretenimento.

### Tela 2
![Tela 2](Tela_2.jpeg )

 3. Tela 3:  Nesta tela apresentamos um exemplo de integração entre os módulos de câmeras de segurança e informações do sistema. Demonstrando uma possibilidade de integração voltada a informações mais especificas/avançadas.

### Tela 3
![Tela 3](Tela_3.jpeg )

## Desafios e Aprendizados

~~Dificuldades Encontradas:~~

## Próximos Passos e Melhorias Futuras

~~Dificuldades Encontradas:~~

## Conclusão

~~Dificuldades Encontradas:~~

## Referências

~~Dificuldades Encontradas:~~
