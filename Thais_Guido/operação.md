# PI3-IFSC-2024-1

Projeto de um dispositivo para auxilio de pessoas com visão debilitada

Acesso rápido:

  - [Introdução](./README.md)
  - [Concepção](./concepção.md)
  - [Design/Projeto](./design.md)
  - [Implementação](./implementação.md)

# Operação

O dispositivo, em sua fase de protótipo, foi testado com a comunidade de pessoas com deficiência visual, permitindo uma avaliação realista de sua funcionalidade. Os testes realizados confirmaram que o protótipo atinge o principal objetivo de melhorar a mobilidade e segurança dos usuários, permitindo a detecção eficiente de obstáculos próximos e fornecendo um feedback claro por meio de vibrações. No entanto, apesar de seu desempenho satisfatório no quesito mobilidade, o protótipo ainda depende de uma fonte de bancada para alimentação, o que limita seu uso contínuo. Essa limitação energética é um ponto crucial que será abordado em futuras otimizações, tanto em relação ao tipo de alimentação quanto à sua autonomia. A seguir, detalhamos as considerações e os testes realizados.

**1. Prototipagem e Tamanho**
Como o dispositivo está em uma fase inicial de desenvolvimento, o tamanho do protótipo ainda não foi otimizado. Embora funcional, ele ocupa mais espaço do que o ideal para um dispositivo que deve ser compacto e discreto, especialmente quando acoplado à bengala do usuário. Em versões futuras, a miniaturização dos componentes eletrônicos será uma prioridade, visando tornar o dispositivo leve e ergonômico, sem comprometer o conforto do usuário durante o uso diário.

**2. Uso de Componentes Simples no Protótipo**

Para a construção do protótipo, optamos por componentes acessíveis e amplamente disponíveis, como 3 sensores ultrassônicos HC-SR04 e um motor vibratório simples. Embora esses componentes tenham se mostrado eficazes durante os testes, eles não são ideais para a versão final do produto.

- **Sensores Ultrassônicos (HC-SR04):** Esses sensores são baratos e fáceis de implementar, porém, possuem limitações, como precisão reduzida em certos tipos de superfícies e possíveis interferências em ambientes com muitos obstáculos próximos. Em futuras versões, planejamos substituir esses sensores por alternativas industriais, como sensores LiDAR ou de maior precisão, que proporcionarão melhor desempenho em detecção de obstáculos.

- **Motor Vibratório:** O motor utilizado atualmente é suficiente para o protótipo, mas consideramos implementar um motor de vibração com melhor controle, que permita ajustes mais finos na intensidade de feedback conforme a proximidade dos obstáculos.

> **2.1 Melhorias Futuras: Componentes Industriais**

A versão final do dispositivo utilizará componentes de nível industrial, mais robustos e duráveis. O objetivo é garantir que o dispositivo funcione em uma variedade de ambientes, incluindo aqueles com alta umidade, terrenos irregulares ou com muitos obstáculos. Sensores mais precisos e motores vibratórios de melhor qualidade permitirão um desempenho mais confiável e eficiente, garantindo uma experiência de usuário ainda mais segura.

**3.Energia e Alimentação**

Um dos maiores desafios encontrados no protótipo atual é a questão da alimentação energética. Todos os testes realizados até o momento foram feitos com uma fonte de bancada ou bateria de 9V, o que permitiu ajustar os parâmetros de funcionamento do dispositivo, mas limita seu uso em condições reais. A bateria de 9V, em particular, oferece uma corrente menor, o que restringe ainda mais a autonomia do dispositivo e impede um uso contínuo por períodos prolongados, enquanto a fonte de bancada, embora ofereça maior controle sobre a corrente e voltagem, não é viável para uso portátil.

> **3.1 Opções de Energia (Pilha ou Bateria)** 

Para tornar o dispositivo totalmente portátil, estamos explorando opções de alimentação por pilhas ou baterias recarregáveis. Entre as opções consideradas:

- Pilhas: São acessíveis e fáceis de substituir, mas têm uma vida útil curta e geram resíduos. Pilhas recarregáveis podem ser uma alternativa melhor, proporcionando uma solução reutilizável.

- Baterias de Íons de Lítio: Baterias recarregáveis de lítio são compactas e oferecem maior autonomia, permitindo o uso contínuo por mais tempo. Essa solução seria ideal para garantir que o dispositivo seja prático no dia a dia, embora exija a implementação de um sistema de recarga, como portas USB.

**4.Testes em Cenários Reais**

Os testes realizados  foram conduzidos em diversos locais da faculdade, incluindo laboratórios utilizando uma fonte de bancada para fornecer uma alimentação estável e controlada durante os experimentos. Estes testes confirmaram a eficácia do dispositivo em melhorar a mobilidade e fornecer um feedback claro e imediato sobre obstáculos próximos.

O feedback dos usuários foi essencial para ajustar a sensibilidade dos sensores e a intensidade da vibração, garantindo uma resposta adequada em diferentes situações e ambientes dentro da faculdade. A experiência acumulada durante esses testes ajudou a identificar áreas para melhorias adicionais, preparando o dispositivo para futuras fases de desenvolvimento.

# *Resultados e Conclusões*

O protótipo atual atingiu os objetivos principais de detecção de obstáculos e melhoria da mobilidade para pessoas com deficiência visual, proporcionando um aumento na segurança ao caminhar em diversos tipos de ambiente. No entanto, devido à dependência de uma fonte de bancada e à falta de uma solução portátil de alimentação, o tempo de operação está limitado. O próximo passo será otimizar a alimentação por bateria e melhorar os componentes para garantir um dispositivo que seja prático, eficiente e durável em seu uso cotidiano.

# Link do vídeo de funcionamento

- https://youtube.com/shorts/w_E45D01ZII?feature=share

- https://youtube.com/shorts/UgbAGNQyC98?feature=share

