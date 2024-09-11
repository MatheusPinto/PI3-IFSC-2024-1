# PI3-IFSC-2024-1

Projeto de um dispositivo para auxilio de pessoas com visão debilitada

Acesso rápido:

  - [Concepção](./concepção.md)
  - [Introdução](./README.md)
  - [Implementação](./implementação.md)
  - [Operação](./operação.md)

# Design 

O desenvolvimento do dispositivo seguiu um processo metódico e estruturado, visando garantir a funcionalidade, a integração eficaz dos componentes e a adaptação ao uso prático. O design do dispositivo foi abordado em várias etapas, desde a montagem inicial do circuito em uma protoboard até a finalização em uma placa de circuito impresso e a inserção em uma case protetora. A seguir, detalhamos cada etapa desse processo de design:

**1. Montagem Inicial do Circuito na protoboard**

Na fase inicial, a montagem do circuito foi feita em uma protoboard para realizar testes e ajustes preliminares. Esta etapa permitiu a validação dos componentes e a configuração do sistema. O processo incluiu os seguintes testes:

- **Testes Individuais dos Sensores Ultrassônicos:**
 
>* **Teste com um Sensor:** Iniciamos com a instalação de um único sensor ultrassônico para avaliar a capacidade de medição de distâncias. Foi verificada a precisão e o alcance do sensor em detectar obstáculos a diferentes distâncias.

>* **Teste com dois Sensores:** Em seguida, foram adicionados dois sensores ultrassônicos para analisar a cobertura e a precisão aumentada na detecção de obstáculos. Testamos a interação entre os sensores e a capacidade de fornecer informações mais detalhadas sobre o ambiente.

>* **Teste com três Sensores:** Finalmente, incorporamos os três sensores ultrassônicos, conforme o design final. Avaliamos a capacidade dos sensores de fornecer uma visão abrangente do ambiente, testando a precisão das medições e a capacidade de identificar a menor distância para priorizar a resposta do motor vibratório.

- **Teste Individual do Motor Vibratório:**

>* Realizamos testes individuais do motor vibratório para ajustar sua resposta tátil. Usamos diferentes tempos de delay para controlar a intensidade e o padrão de vibração. Este teste ajudou a calibrar o motor para fornecer feedback tátil adequado em resposta à proximidade dos obstáculos.

- **Integração Sensor e Motor:**

>* **Implementação Inicial:** Começamos integrando um sensor ultrassônico com o motor vibratório, avaliando como o motor respondia às distâncias medidas pelo sensor.

>* **Integração Completa:** Posteriormente, integramos os três sensores ultrassônicos com o motor vibratório. O sistema foi ajustado para que o motor vibrasse com base na menor distância detectada entre os sensores, indicando o obstáculo mais próximo e permitindo uma resposta tátil precisa para o usuário.

Abaixo, é possível visualizar imagens do circuito montado na protoboard, ilustrando a configuração inicial e os testes realizados.

![Protoboard](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/protoboard1.jpeg)

![Protoboard](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/protoboard2.jpeg)

![Protoboard](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/protoboard3.jpeg)

**2. Desenvolvimento da Placa de Circuito Impresso (PCI)**

Após a validação e ajuste do circuito na protoboard, o próximo passo essencial foi o desenvolvimento da Placa de Circuito Impresso (PCI). Este processo envolveu a criação de uma placa personalizada para montar os componentes eletrônicos de forma organizada e robusta. O desenvolvimento da PCI trouxe várias vantagens significativas em relação à protoboard. A PCI oferece uma base sólida e durável, essencial para a confiabilidade e longevidade do dispositivo, reduzindo o risco de falhas e desconexões típicas das conexões temporárias. Além disso, a PCI possibilita uma disposição compacta e organizada dos componentes eletrônicos, resultando em um design mais limpo e eficiente que facilita a integração dos sensores ultrassônicos, microcontrolador e motor vibratório. A disposição otimizada das trilhas na PCI minimiza interferências eletromagnéticas e melhora o desempenho geral do circuito. Por fim, a PCI simplifica a montagem e manutenção do dispositivo, com componentes fixos e claramente identificáveis, tornando o processo de ajuste e reparo mais ágil e preciso.

Para o design da PCI, utilizamos o software KiCad, uma ferramenta poderosa e versátil para criação de circuitos impressos. O KiCad permitiu a elaboração detalhada do esquemático e a criação do layout da placa, assegurando que todos os componentes fossem posicionados e conectados de maneira eficiente. O software facilitou a visualização e a verificação do circuito antes da fabricação, garantindo que o projeto atendesse a todas as especificações e requisitos necessários. Abaixo, é possível visualizar o esquemático e o layout da PCI, além do passo a passo da fabricação da placa, desde o design até a montagem final.

![KICAD](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/KICAD1.jpeg)

![KICAD](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/KICAD2.jpeg)

![KICAD](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/KICAD3.jpeg)

![PCI](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/PCI1.jpeg)

![PCI](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/PCI2.jpeg)

![PCI](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/PCI3.jpeg)

![PCI](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/PCI4.jpeg)


**3.Integração na Case**

Após a conclusão da Placa de Circuito Impresso (PCI), a próxima etapa crucial foi a integração do circuito na case protetora. Para esta fase, optou-se por utilizar um material leve e prático: o papel número 30. Esta escolha não só assegura um design leve e funcional, mas também é uma solução econômica e eficiente para a montagem do dispositivo.

O papel foi selecionado por sua combinação de leveza e resistência, características essenciais para garantir que o dispositivo se mantenha firme e estável quando acoplado à bengala, sem adicionar peso significativo. Além disso, o material oferece uma boa capacidade de personalização, permitindo que a case seja moldada e adaptada às dimensões e características específicas do circuito e dos componentes.

O processo de integração envolveu a criação de uma estrutura projetada para acomodar a PCI de maneira segura e organizada. A case foi projetada para proteger o circuito contra impactos e condições ambientais adversas, além de garantir que todos os sensores e o motor vibratório estejam posicionados de forma eficaz para o seu funcionamento adequado.

Abaixo, são apresentadas imagens da case, incluindo o processo de montagem e a integração final com a Placa de Circuito Impresso. Estas imagens ilustram como o circuito foi inserido na case e como a solução em papel contribui para a funcionalidade e a estética final do dispositivo.

![Case](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/case1.jpeg)

![Case](https://github.com/thaislisatchok/PI3-IFSC-2024-1/blob/main/Thais_Guido/imagens.md/case2.jpeg)
