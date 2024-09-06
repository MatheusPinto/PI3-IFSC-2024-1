# PI3-2024 - AI FOOD -  Sistema de detecção de alimentos com inteligência artificial integrado com balança

**INSTITUTO FEDERAL DE SANTA CATARINA**

**Unidade Curricular:**  Projeto Integrador 3

**Professor:**  Robinson Pizzio e Matheus Leitzke Pinto

**Alunos:**  Eric Monteiro dos Reis e Matheus Sandim Gonçalves

# Sumário

[Introdução](#introdução)

[Tecnologias utilizadas](#tecnologias-utilizadas)

  * [Python](#Python)
  * [YOLOv8](#YOLOv8)
  * [MicroPython](#MicroPython)
  * [AmazonRDS](#Amazon-RDS)

[Desenvolvimento e resultados](#Desenvolvimento-e-resultados)
  * [Treinamento da Inteligência Artificial](#Treinamento-da-Inteligência-Artificial)
  * [Hardware](#Hardware)
  * [Software](#Software)

[Dificuldades](#Dificuldades)

[Passos Futuros](#Passos-futuros)

[Referências bibliográficas](#Referências-bibliográficas)

## Introdução
Este projeto propõe a utilização de inteligência artificial (IA) para monitorar o consumo calórico diário dos usuários, para isso integra um modelo de IA com uma câmera de celular e uma balança, permitindo que os usuários identifiquem automaticamente os alimentos que consomem e, com base no peso detectado pela balança, obtenham informações nutricionais detalhadas, como a quantidade de proteínas, carboidratos e gorduras. Esses dados são então armazenados em um banco de dados na nuvem, onde um resumo calórico é gerado para cada usuário, proporcionando um acompanhamento preciso e contínuo de sua dieta diária.

## Tecnologias utilizadas

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/Tecnologias.png)

### Python
Python é uma linguagem de programação de software muitas vezes utilizadas amplamente utilizada em diversas áreas, como desenvolvimento de software, automação de tarefas, análise de dados, aprendizado de máquina, etc. É uma linguagem de propósito geral conhecida pela sua sintaxe simples e fácil de ler, pois se assemelha muito a linguagem natural. Sua versatilidade e facilidade de prototipação rápida faz com que ela seja uma das linguagens de programação mais utilizadas da atualidade.

### YOLOv8
YOLOv8 (You Only Look Once version 8) é um algoritmo de detecção de objetos em imagens. Embora o YOLOv8 não seja a versão mais recente, ele foi selecionado para este projeto devido à ampla disponibilidade de tutoriais e exemplos práticos, além de sua comprovada eficiência e rapidez na detecção de objetos em vídeos.

### MicroPython
MicroPython é uma implementação enxuta da linguagem Python projetada para rodar em microcontroladores e dispositivos embarcados com recursos limitados, como ESP32 e ESP8266. MicroPython oferece acesso a hardware, como pinos GPIO, sensores, e comunicação Wi-Fi, e inclui bibliotecas para tarefas comuns em sistemas embarcados.

### Amazon RDS
O Amazon RDS (Relational Database Service) é um serviço gerenciado da AWS que facilita a configuração, operação e escalabilidade de bancos de dados relacionais na nuvem. O RDS automatiza tarefas como backup, atualização de software, escalabilidade de hardware e recuperação de falhas, permitindo que os usuários se concentrem na otimização de suas aplicações.

# Desenvolvimento e resultados

## Treinamento da Inteligência Artificial
Para treinar a IA a reconhecer diferentes alimentos, selecionamos para este projeto 15 alimentos comuns presentes no dia a dia do brasileiro.

- Alface
- Arroz
- Banana
- Batata
- Carne vermelha
- Cebola
- Feijão
- Carne de frango
- Laranja
- Leite
- Maçã
- Melancia
- Morango
- Ovo
- Tomate

O dataset utilizado contém um total de 1500 imagens, distribuídas de maneira a garantir uma cobertura adequada dos diferentes alimentos.

- **Treinamento (70%):** 1050 imagens
- **Validação (20%):** 300 imagens
- **Teste (10%):** 150 imagens

Esta divisão é considerada uma boa prática, recomendada para garantir que o modelo seja treinado, validado e testado adequadamente.

Para a rotulagem dos dados, utilizamos a ferramenta CVAT (Computer Vision Annotation Tool). CVAT é uma ferramenta online de anotação e rotulação de imagens, projetada para facilitar a marcação precisa de objetos em imagens e vídeos para serem utilizadas no treinamento de inteligências artificiais. No nosso caso, a ferramenta foi empregada para marcar os alimentos nas imagens com retângulos delimitadores, que indicam a localização e a classe de cada alimento.

> [!CAUTION]
> O CVAT oferece para o usuário a possibilidade de rotular suas imagens de diversas formas, incluindo a delimitação clicando em pontos na imagem para definir a área a ser rotulada. No entanto, para sistemas treinados utilizando o YOLOv8, o Yolov8 **SOMENTE ACEITA RETÂNGULOS DELIMITADORES**. Embora outras formas de demarcação não causem problemas durante o processo de exportação dos rótulos, elas não são reconhecidas pelo YOLOv8 durante o treinamento. Esse detalhe foi descoberto somente após a demarcação de inúmeras imagens sem utilizar retângulos.

Abaixo está um exemplo de como os rótulos são marcados no CVAT:

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/CVAT.png)

As imagens utilizadas para o treinamento e validação continham labels indicando a presença e identificação do alimento. Já nas imagens de teste, os labels não estavam presentes, sendo a tarefa da IA localizar e identificar corretamente os alimentos.

O treinamento do YOLOv8 envolve os seguintes passos:

- **Pré-processamento dos Dados:** As imagens foram ajustadas para o tamanho apropriado e os rótulos foram convertidos para o formato exigido pelo YOLOv8.

- **Configuração do Modelo:** O modelo YOLOv8 foi configurado com parâmetros específicos, como o número de classes (neste caso, 15 tipos de alimentos) e o número de epochs (medida de tempo utilizada para indicar a quantia de ciclos que o sistema passou durante o treinamento. Foram feitos testes com diversos valores de epochs diferentes chegando a um valor de até 300 epochs.

- **Treinamento:** O modelo foi treinado com as imagens de treinamento, ajustando seus pesos para minimizar a diferença entre as previsões e as anotações reais. Durante o treinamento, o modelo é exposto a diversas imagens e ajusta seus parâmetros para melhorar a detecção de alimentos.

- **Validação:** Durante o treinamento, o modelo foi validado com o conjunto de dados de validação para monitorar sua performance e evitar overfitting. A validação ajuda a garantir que o modelo generalize bem para novas imagens.

- **Teste:** Após o treinamento, o modelo foi testado com o conjunto de dados de teste para avaliar sua precisão final e desempenho em detectar alimentos em imagens nunca vistas antes.

Os gráficos a seguir ilustram os resultados obtidos com o melhor treinamento realizado:

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/F1_curva.png)

Mostra a pontuação F1 (média harmônica de precisão e recuperação) em diferentes limites de confiança. Um pico mais alto sugere melhor desempenho do modelo.

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/CFN.png)

Na diagonal principal, observa-se o quanto o algoritmo acertou na identificação dos alimentos. As cores mais claras indicam valores de predição mais baixos, resultantes de confusões com outros alimentos ou com o fundo da imagem. Exceto por maçã e cebola, os demais alimentos apresentaram uma precisão de pelo menos 70%, o que é uma média considerável. Podemos observar isso na prática na figura abaixo, onde a IA consegue reconhecer todos os alimentos apresentados.

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/teste_IA.png)

Outro aspecto relevante é a análise do último gráfico, que apresenta a precisão de cada alimento de forma numérica. Observa-se que, em alguns casos, a IA confunde alimentos entre si ou com o fundo da imagem. A confusão com o background (fundo) poderia ser mitigada caso imagens sem labels tivessem sido incluídas no treinamento.

![](https://github.com/suzuki1994/PI3-2024/blob/a0ba1a532e0eedd0a04ab466d8b57646ec022bbe/Figuras/resultado.jpg)

## Hardware

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/Componentes.png)

O hardware deste projeto consiste em uma balança digital com comunicação via Wi-Fi. Ela funciona a partir do sinal enviado por uma célula de carga, que é um transdutor resistivo capaz de converter a força aplicada em um sinal elétrico. No GIF abaixo, é demonstrada a simulação do funcionamento da célula de carga, onde uma força peso é aplicada, causando uma leve compressão na parte superior do sistema. Isso gera um sinal elétrico, que é amplificado pelo módulo HX711. Esse sinal é então enviado para o microcontrolador ESP32.

![](https://www.toledobrasil.com/blob/upload/peso-padrao-2.gif)

O HX711 é um módulo conversor e amplificador de 24 bits que junto com uma célula de carga e um microcontrolador é possível montar uma balança digital. A célula de carga utilizada neste projeto é de uma 1KG, porém nada impede de substituir ela por uma de 3KG ou 5KG. Ambos foram escolhidos para este projeto por serem vendidos em forma de kit, oferecerem um bom custo-benefício e contarem com diversos exemplos e tutoriais disponíveis na internet.

![](https://github.com/suzuki1994/PI3-2024/blob/d35aea34d81d1e8f14764084d93dd8fd95840884/Figuras/Celula_de_carga.png)

O HX711 possui um lado que deve ser conectado com a célula de carga (E+ E- A- A+ B- B+), onde E+ (VCC fio vermelho) e E- (GND fio preto) e as saídas (A+ A-) e (B+ e B-), onde utilizamos as saídas A+ (fio verde) e A- (fio branco).
O outro lado onde tem os pinos (GND DT SCK VCC) são conectados com o microcontrolador (ESP32).
Neste projeto o pino 2 do ESP32 é o DT (data) e o pino 4 é o SCK (clock). Alimentação é 3.3V

![](https://github.com/suzuki1994/PI3-2024/blob/282f6ae8c8375911f87fdac173b76ee60df269f3/Figuras/HX711.jpg)

Para melhor visualização do valor medido pela balança foi utilizado um display de LCD 16x2 com um módulo I2C para comunicar com o ESP32. Os pinos utilizados foram o 22 (SCL) e o pino 21 (SCA). Já possuiamos este display o que facilitou na sua escolha para o projeto.

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/LCD_16x2_m%C3%B3dulo_I2C.png)

A célula de carga foi montada em duas placas de plástico que foram recicladas de uma antiga placa, a figura abaixo podemos oberservar a visão lateral da balança

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/Balan%C3%A7a%20side%20view.jpeg)

A figura abaixo mostra a balança em pleno funcionamento. Os dados são enviados para um IP por meio de uma conexão Wi-Fi. Para isso, o ESP32 foi programado para atuar como um servidor web (webserver).

![](https://github.com/suzuki1994/PI3-2024/blob/a0ba1a532e0eedd0a04ab466d8b57646ec022bbe/Figuras/Balan%C3%A7a_funcionando.png)

## Software

### Amazon RDS
Para a implementação do banco de dados utilizado no projeto, primeiro é necessário possuir uma conta no AWS. O Amazon RDS é um serviço que possui plano gratuito, onde é possível criar bancos de dados sem custos, porém dependendo da configuração utilizada ao criar o banco, o serviço pode possuir gastos relevantes.

Neste projeto, utilizamos uma conta do AWS educacional, obtida através de parceria com o IFSC. Para este projeto optamos pela utilização de um banco de dados relacional, visto que é necessário o armazenamento da informação de diversas entidades, com elas se relacionando entre si.

#### Passos para a criação do banco de dados no AWS RDS:
1. Realizar o login na conta do AWS.
2. Buscar por RDS na busca de serviços localizada no topo da página.
3. No painel do RDS, arraste para baixo até encontrar a opção "Create Database".
4. Selecione o Engine (utilizamos o PostgreSQL).
5. Em Template selecione "Free tier".
6. Em Settings configure o nome da instância do banco de dados, um usuário admin e a senha.
7. Em conectividade garanta que na parte de Publicly accessible está marcado a opção "Yes", para que possamos acessar o banco de dados remotamente.
8. Escolha um nome para o banco de dados em Additional Configuration
9. O resto das opções foi utilizada as padrões.
10. Clicke em "Create Database".

Após alguns minutos, o banco de dados estará disponível para conexão.

#### Ferramentas utilizadas para comunicação com o banco de dados:
- **Psycopg2**:  É um adaptador popular e amplamente utilizado para conectar aplicações Python a bancos de dados PostgreSQL. Utilizando essa biblioteca podemos executar comandos SQL, permitindo que possamos interagir de forma eficiente com o banco de dados via código Python.

- **psql**: É uma interface de linha de comando utilizado para interagir com o banco de dados e utilizamos ela principalmente para debug e testes do banco de dados. Segue abaixo uma imagem demonstrando a visualização de uma das tabelas utilizadas no projeto.

![](https://github.com/suzuki1994/PI3-2024/blob/main/Figuras/Banco%20de%20dados%20de%20alimentos%20via%20psql.png)

#### Estrutura do banco de dados:
A arquitetura do banco de dados foi desenhada com três tabelas principais:
- **Tabela de Usuários**: Contém as informações de identificação dos usuários.
- **Tabela de Alimentos**: Armazena dados dos alimentos cadastrados.
- **Tabela de Alimentos Consumidos**: Registra os alimentos consumidos pelos usuários.

##### Tabela de usuários
A tabela de usuários possui os seguintes campos:
- `id`: Identificador único do usuário.
- `email`: Endereço de e-mail do usuário.
- `password`: Senha de acesso do usuário.
- `created_at`: Data de criação do registro.

O e-mail e a senha são utilizados para autenticar o usuário no sistema. O `id` é utilizado como chave estrangeira em outras tabelas, garantindo a associação correta entre os dados de usuários e suas interações com os alimentos.

##### Tabela de alimentos
A tabela de alimentos armazena informações sobre os alimentos cadastrados no sistema. A tabela de alimentos possui os seguintes campos:
- `id`: Identificador único do alimento.
- `food_name`: Nome do alimento (ex: maçã, arroz).
- `variety`: Variedade específica do alimento (ex: gala, integral).
- `protein_per_gram`: Quantidade de proteína por grama do alimento, com até duas casas decimais.
- `carbs_per_gram`: Quantidade de carboidratos por grama do alimento, com até duas casas decimais.
- `fats_per_gram`: Quantidade de gordura por grama do alimento, com até duas casas decimais.
- `calories_per_gram`: Quantidade de calorias por grama do alimento, com até duas casas decimais.
- `last_updated`: Armazena a data e hora da última atualização do alimento. Por padrão, o valor é a data e hora atuais.
- `additional_info`: Campo JSON que armazena informações adicionais sobre o alimento, como por exemplo quantidade de micro-nutrientes como potássio, magnésio, vitaminas, etc.

> **Observação:** Foi criado uma restrição ao criar a tabela ,de forma a garantir que a combinação de `food_name` e `variety` seja sempre única na tabela.

##### Tabela de diário alimentar
A tabela de diário alimentar armazena os registros de consumo de alimentos por usuários. A tabela de diário alimentar possui os seguintes campos:
- `id`: Identificador único da entrada de diário.
- `user_id`: Identificador do usuário que realizou o registro, referenciando a tabela de usuários.
- `food_id`: Identificador do alimento consumido, referenciando a tabela de alimentos.
- `amount_grams`: Quantidade de alimento consumido em gramas, com até duas casas decimais.
- `entry_date`: Data em que o alimento foi consumido.
- `total_protein`: Quantidade total de proteína consumida, calculada com base na quantidade de alimento em gramas, com até duas casas decimais.
- `total_carbs`: Quantidade total de carboidratos consumidos, com até duas casas decimais.
- `total_fats`: Quantidade total de gordura consumida, com até duas casas decimais.
- `total_calories`: Quantidade total de calorias consumidas, com até duas casas decimais.

> **Observação:** `food_id` foi adicionado como chave estrangeira, referenciando à tabela de alimentos e garantindo a integridade entre os alimentos registrados e os consumidos.

### Implementação de CRUD de operações

Após estabelecer a estrutura do banco de dados, foi desenvolvido um CRUD (Create, Read, Update, Delete) para gerenciar as operações essenciais utilizando `psycopg2`. Este CRUD serve como uma interface de comunicação com o banco de dados PostgreSQL, permitindo a execução das seguintes operações:

- **Criação (Create):** Adiciona novos registros no banco de dados, como novos alimentos, usuários ou entradas de diário alimentar.
- **Leitura (Read):** Recupera e exibe dados armazenados, como informações sobre alimentos ou o histórico de consumo dos usuários.
- **Atualização (Update):** Modifica registros existentes, permitindo, por exemplo, atualizar os detalhes de um alimento ou ajustar as informações de login do usuário.
- **Deleção (Delete):** Remove registros do banco de dados, como a exclusão de alimentos ou a remoção de entradas do diário alimentar.

Essas operações foram implementadas para facilitar a interação do usuário com o projeto, garantindo que todas as ações necessárias sejam suportadas de maneira eficiente e eficaz. As funções que performam as operações do CRUD se encontram nos arquivos ```food_diary_crud.py()```, ```users_crud.py()``` e ```ingredients_crud.py()```.

#### Conexão Remota com o Banco de Dados

Após criar o CRUD, desenvolvemos uma função para conectar ao banco de dados remotamente, permitindo a interação com ele. O código utilizado para essa conexão é o seguinte:

```python
import psycopg2
import config as creds

PG_ENDPOINT = "pi3-db.cvridvfsmzai.us-east-1.rds.amazonaws.com"
PG_DATABASE_NAME = "projeto_integrador_3_db"
PG_USERNAME = "postgres"
PG_PASSWORD = ""

# NOTE (@eric_reis): Comando para acessar o banco de dados utilizando a lib "psql":
# psql \
#    --host=pi3-db.cvridvfsmzai.us-east-1.rds.amazonaws.com \
#    --port=5432 \
#    --username=postgres \
#    --password \
#    --dbname=projeto_integrador_3_db

def connect():
    connection_config = (
        "host="
        + creds.PG_ENDPOINT
        + " port="
        + "5432"
        + " dbname="
        + creds.PG_DATABASE_NAME
        + " user="
        + creds.PG_USERNAME
        + " password="
        + creds.PG_PASSWORD
    )

    print("\nInitializing connection to:")
    print(connection_config)
    conn = psycopg2.connect(connection_config)
    print("Connected")
    cursor = conn.cursor()

    return conn, cursor
````
A função ```connnect()``` configura a conexão com o banco de dados utilizando as credenciais do banco de dados criado na AWS e estabelece um cursor para a execução de comandos SQL. Ela é utilizada em diversos pontos do projeto para estabelecer a conexão com o banco.

O comando comentado foi o comando alternativo de debug e testes utilizado para acessar o banco de dados utilizando a ferramenta ```psql```.

### Fluxo Geral do Projeto

Após o treinamento da inteligência artificial, o desenvolvimento do hardware necessário e a implementação do banco de dados, passamos para a implementação do fluxo geral do projeto. O fluxo consiste nas seguintes fases:

1. **Fase de Login e Validação do Usuário**
   - Ao iniciar o projeto, o usuário escolhe se deseja fazer login em uma conta existente ou criar uma nova conta. Essa interação é gerenciada através de um menu exibido no prompt de linha de comando.
   - Após a validação do usuário, é estabelecida a conexão com o banco de dados na AWS e a câmera do celular ou computador do usuário é aberta para iniciar a captura das imagens.

2. **Detecção de Imagens de Forma Contínua**
   - Utilizamos a biblioteca OpenCV para o processamento e captura contínua das imagens. O OpenCV é amplamente utilizado em visão computacional.
   - Cada frame capturado é analisado pelo modelo de detecção de alimentos treinado com YOLOv8. Se um alimento é detectado com uma precisão superior a 90% (valor escolhido pela equipe e passível de alteração), buscamos esse alimento no banco de dados.

3. **Resposta do Sistema ao Identificar um Alimento**
   - A identificação de um alimento resulta em uma busca no banco de dados com base no nome do alimento detectado. A partir das informações retornadas, passamos para a fase de seleção pelo usuário.

4. **Seleção pelo Usuário da Opção de Alimento Desejado e Pesagem do Alimento**
   - Utilizando o OpenCV, exibimos na tela as opções de variedades do alimento detectado, cada uma com um valor numérico prefixo. O usuário deve selecionar a opção desejada pressionando a tecla com o número correspondente.
   - Em seguida, é feita uma requisição para a balança para obter a leitura do peso do alimento.

5. **Salvar os Resultados no Banco de Dados**
   - Após a seleção da variedade do alimento, o sistema calcula as quantidades de proteínas, gorduras e carboidratos consumidos com base no peso do alimento e nas informações armazenadas no banco de dados.
   - Uma entrada é salva no diário alimentar para o dia específico, e o total de calorias consumidas naquele dia é atualizado. No canto inferior direito da tela, é exibido o total de calorias consumidas naquele dia.

#### Funcionalidades Adicionais

Além do fluxo principal, o sistema possui algumas teclas que desempenham funções importantes para melhorar a experiência do usuário:

- **Tecla 'T':** Lista o total de proteínas, carboidratos e gorduras consumidas para o dia inteiro em detalhes.
- **Tecla 'C':** Cancela a seleção da variedade de um alimento, permitindo que o sistema retome a detecção de novos alimentos. Essa tecla é útil para descartar leituras errôneas de alimentos detectados.
- **Tecla 'Q':** Finaliza o projeto e fecha a janela de captura de imagens.

Essas funcionalidades adicionais proporcionam maior controle e flexibilidade durante o uso do sistema, melhorando a experiência do usuário.

## Dificuldades

Durante o desenvolvimento do projeto, enfrentamos algumas dificuldades que foram parcialmente resolvidas e outras que foram registradas para futuras atualizações. Abaixo estão algumas das principais dificuldades encontradas:

- **Integração com a balança:** Ao integrar a balança com o software do projeto para enviar o peso do alimento medido via Wi-Fi, enfrentamos desafios com o protocolo de comunicação. Após várias tentativas, estabelecemos uma estrutura funcional usando Sockets de comunicação e requisições HTTP para a troca de informações entre hardware e software. No entanto, o IP do ESP32 mudava dependendo da rede em que estava conectado, o que exigia ajustes manuais durante a conexão com redes novas.

- **Problemas com o dataset:** A IA teve dificuldades em distinguir entre alimentos visualmente semelhantes, como tomates e morangos, ou até mesmo confundindo leite com paredes brancas no fundo das imagens. Para mitigar esses problemas, treinamos o modelo com variações das imagens originais, incluindo rotações e alterações na claridade e no HUE das cores. Embora essas medidas tenham melhorado a precisão, acreditamos que um dataset maior, com imagens de fundo (imagens sem rótulo), poderia aumentar ainda mais a precisão da IA.

- **Implementação do banco de dados na AWS:** A principal dificuldade foi a implementação do banco de dados na AWS, uma tecnologia que tivemos que aprender do zero. A integração do banco de dados no projeto, incluindo a atualização, busca e adição de informações em tempo real, também se mostrou desafiadora. Esse processo resultou em várias tentativas e erros, mas proporcionou uma valiosa experiência de aprendizado.

## Passos futuros

Para aprimorar o projeto, acreditamos que os seguintes passos poderiam melhorar a qualidade do projeto:

- **Construir um gabinete para a balança:** Criar uma case protetora para a balança, garantindo maior durabilidade e integridade dos componentes.

- **Corrigir estilo de leitura da balança:** Devido as dificuldades ao implementar a integração entre a balança e o software do projeto, não foi possível corrigir a tempo o problema das leituras da balança serem feitas somente quando o usuário seleciona a variedade do alimento. Ao aplicar a leitura constante do peso melhorariamos o uso no dia a dia do projeto para o usuário.

- **Analisar a viabilidade da substituição da comunicação com a balança para Bluetooth:** A utilização do Bluetooth poderia reduzir a necessidade de ajustes manuais, melhorando a experiência do usuário e a eficiência do sistema.

- **Expandir o dataset de treinamento:** Aumentar o tamanho do dataset de treinamento com mais imagens, incluindo imagens que representem melhor o dia a dia do brasileiro e imagens de fundo para melhorar a precisão do modelo.

- **Desenvolver uma interface para o usuário:** Criar uma interface gráfica para o usuário, proporcionando uma experiência mais intuitiva e fácil de usar para interagir com o sistema.


## Referências bibliográficas

ULTRALYTICS. YOLOv8 - Ultralytics. Disponível em: https://docs.ultralytics.com/models/yolov8/. Acesso em: 22 abr. 2024.

MICROPYTHON. MicroPython Documentation. Disponível em: https://docs.micropython.org/en/latest/esp32/tutorial/intro.html. Acesso em: 22 abr. 2024.

ULTRALYTICS. Configuring Your Instance - YOLOv5 AWS Quickstart Tutorial. Disponível em: https://docs.ultralytics.com/yolov5/environments/aws_quickstart_tutorial/#configuring-your-instance. Acesso em: 22 abr. 2024.

JACKSON, Sam. Using RDS on AWS with Jupyter Notebooks. Medium, 6 ago. 2018. Disponível em: https://medium.com/@sjacks/using-rds-on-aws-with-jupyter-notebooks-c2703299fcc8. Acesso em: 15 ago. 2024.
