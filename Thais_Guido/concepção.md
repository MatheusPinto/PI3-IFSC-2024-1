# PI3-IFSC-2024-1

Projeto de um dispositivo para auxilio de pessoas com visão debilitada

Acesso rápido:

  - [Introdução](./README.md)
  - [Design/Projeto](./design.md)
  - [Implementação](./implementação.md)
  - [Operação](./operação.md)

# Concepção

## Intuito do Projeto:

Desenvolver um dispositivo inteligente que possa ser acoplado às bengalas tradicionais utilizadas por pessoas com deficiência visual. Este dispositivo tem como objetivo melhorar a mobilidade, segurança e independência dos usuários ao fornecer assistência em tempo real na detecção de obstáculos e navegação em ambientes variados.

## Objetivos :

 -   Aprimorar a funcionalidade das bengalas tradicionais, tornando-as inteligentes e capazes de oferecer feedback útil aos usuários.
 -   Facilitar a locomoção e aumentar a segurança das pessoas com deficiência visual durante seus deslocamentos diários.
 -   Promover a inclusão e autonomia, permitindo que os usuários naveguem com mais confiança em diferentes ambientes.

## Relevância e Impacto Social :

A mobilidade é um dos principais desafios enfrentados por pessoas com deficiência visual, sejam elas cegas ou com baixa visão. A bengala tradicional é uma ferramenta essencial que oferece suporte e orientação, mas possui limitações na detecção de obstáculos acima do solo ou em movimento. Para compreender melhor as necessidades específicas desse público, foi realizada uma pesquisa inicial com pessoas que possuem diferentes graus de perda visual.

Os resultados dessa pesquisa destacaram a importância de desenvolver um dispositivo que pudesse ser integrado às bengalas já utilizadas pelos usuários. Cada pessoa possui um tipo específico de bengala, adaptada às suas preferências e necessidades. Portanto, criar um acessório acoplável faz mais sentido do que propor a substituição completa da bengala. Isso garante que os usuários possam manter o uso de seus equipamentos familiares, adicionando funcionalidades inteligentes sem comprometer o conforto ou a eficácia.

Além disso, ao optar por um dispositivo acoplável, o projeto promove a inclusão ao considerar a diversidade de usuários e suas preferências individuais. Essa abordagem facilita a adoção da tecnologia, reduz custos e respeita a individualidade de cada pessoa, aumentando o potencial de impacto social positivo.

O desenvolvimento deste dispositivo não só aprimora a segurança e a independência dos usuários, mas também contribui para a conscientização sobre a importância de tecnologias assistivas personalizáveis. Ao melhorar a interação das pessoas com deficiência visual com o ambiente ao seu redor, o projeto promove a inclusão social e destaca a relevância de soluções inovadoras no campo da acessibilidade.

## Pesquisa inicial:

Para garantir que o dispositivo atenda às necessidades reais das pessoas com deficiência visual, foi realizada uma pesquisa preliminar com pessoas cegas e com baixa visão, além de profissionais da área de acessibilidade. O objetivo dessa pesquisa foi identificar as principais dificuldades enfrentadas durante a locomoção e entender quais funcionalidades poderiam ser integradas ao dispositivo para aumentar sua eficácia e praticidade.

A pesquisa também incluiu um levantamento detalhado dos diferentes tipos de bengalas disponíveis no mercado, desde as bengalas tradicionais até as versões eletrônicas mais avançadas. Foi observado que as bengalas eletrônicas, embora ofereçam vantagens como a detecção de obstáculos e o feedback por meio de vibrações ou sons, têm um custo muito elevado, o que as torna inacessíveis para grande parte das pessoas com deficiência visual. A questão do preço foi levantada por diversos entrevistados, que destacaram a necessidade de uma solução financeiramente viável.

Além disso, a pesquisa mostrou que muitos usuários preferem continuar utilizando suas bengalas tradicionais, adaptadas às suas preferências pessoais, e demonstraram resistência à ideia de substituí-las por dispositivos eletrônicos complexos ou caros. Assim, a decisão de desenvolver um acessório que pudesse ser acoplado a qualquer bengala, adicionando funcionalidades inteligentes de forma simples e prática, foi bem recebida. Essa abordagem permite que os usuários mantenham o uso de suas bengalas atuais, enquanto se beneficiam de um dispositivo inteligente de baixo custo.

A preferência por sinais táteis, como vibrações, também foi reforçada, já que esses sinais não interferem com a audição, que é um sentido crucial para a navegação. Esses insights foram fundamentais para orientar o design e as funcionalidades do dispositivo, garantindo que ele seja acessível, fácil de usar e adequado às necessidades reais dos usuários.

## Definição do sistema proposto:

A partir dos insights obtidos durante a pesquisa inicial e o estudo de casos, foi possível definir a estrutura do dispositivo e os componentes necessários para o desenvolvimento do protótipo. Com o foco em criar uma solução acessível, eficiente e de fácil adaptação às bengalas tradicionais, chegamos à conclusão de que o dispositivo precisaria ser simples e de baixo custo, mantendo a funcionalidade de detecção de obstáculos e fornecimento de feedback tátil. A seguir, estão os principais componentes escolhidos para a concepção do protótipo:

## Lista de componentes :

| Componentes                          |Representação       |
| -------------------------------------|------------------- |
| Microcontrolador STM32F103C8         | ![stm32f103](https://ae01.alicdn.com/kf/Sd6e543a09d0648a8912329bc7725d665w.jpg_640x640Q90.jpg_.webp) |
| Sensor Ultrassônico HC-SR04          | ![ultrassonico](https://cdn.awsli.com.br/600x700/78/78150/produto/2888532/62bc744cec.jpg) |
| Motor 3V~6V                          | ![motor](https://cdn.awsli.com.br/380x380/468/468162/produto/1941431669db977161.jpg)      |        
| Driver para o motor                  | ![driver](https://rukminim2.flixcart.com/image/416/416/k44hksw0/transistor/p/r/n/20-pics-2n2222-npn-transistor-sme-original-imafn327jjweqgzf.jpeg?q=70&crop=false)|
| Regulador de tensão                  | ![regulador](https://www.ryndackcomponentes.com.br/media/catalog/product/cache/1/image/9df78eab33525d08d6e5fb8d27136e95/i/m/image_5423.jpg)| 


