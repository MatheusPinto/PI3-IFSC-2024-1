<table align="center"><tr><td align="center" width="9999"><br>
<img src="logoifsc.png" align="center" width="150" alt="Logo IFSC">

# Smart Mirror

<b><br>Instituto Federal de Educação, Ciência e Tecnologia de Santa Catarina<br>
Campus Florianópolis<br>
Departamento Acadêmico de Eletrônica<br>
Projeto Integrador III</b>

*Jonathan Chrysostomo Cabral Bonette*<br>
*Alexsander Vieira*

#

O espelho inteligente é um dispositivo integrado com funcionalidades interativas e personalizáveis, capaz de oferecer uma variedade de serviços e informações enquanto o usuário se prepara para o dia ou realiza tarefas cotidianas em frente ao espelho.

</td></tr></table>

# Conceito

• Exibição de informações uteis, como previsão do tempo, calendário, noticias, entre outros.<br>
• Integração de assistentes virtuais para auxiliar nas tarefas diárias e responder a consultas do usuário.<br>
• Monitoramento de saúde e bem-estar por meio de sensores integrados.<br>
• Capacidade de reprodução de música e vídeos durante o uso do espelho.<br>
• Personalização da interface e das informações exibidas de acordo com as preferências do usuário.<br>
• Capacidade interativa através de sensores de movimentos (hands gestures).<br> 

# Benefícios do Projeto

• Melhoria da eficiência e produtividade do usuário ao oferecer acesso rápido a informações relevantes.<br>
• Aumento do conforto e conveniência no ambiente residencial.<br>
• Exploração de novas possibilidades de interação entre humanos e tecnologia.<br>

# Recursos Necessários: Hardware

• Tela (televisão ou monitor com caixinha de som para reprodução sonora).<br>
• Placa de desenvolvimento (raspberry pi pelo poder de processamento necessário).<br>
• Sensores de movimentos (MGC3130, GP2Y0A21YK, PAJ7620u2, APDS9960, PiCamera*).<br>
• Moldura IR.<br>
• Entre outros.<br>

# Recursos Necessários: Software

• linguagens de programação como JavaScript, Node.js, e frameworks para desenvolvimento de interfaces.<br>
• JavaScript para configurar a interface front web.<br>
• Node para configurar o back end e os módulos necessários.<br>
• API’s diversas para os funcionamentos dos módulos.<br>
• Raspbian (OS do raspberry pi).<br>

# Fluxograma de Software

• 1. Organizar os pré requisitos necessários<br>
• 2. Configurar o sistema operacional (Raspbian)<br>
• 3. Instalar e configurar o servidor back-end (Node.js)<br>
• 4. Desenvolver e configurar a interface front-end (JavaScript)<br>
• 5. Desenvolver os blocos necessários para a nossa aplicação<br>
• 6. Conectar e configurar as APIs necessárias para os blocos<br>
• 7. Testar e aprimorar a funcionalidade dos blocos sem os recursos de sensores<br>
• 8. Implementar os recursos de sensores, alimentar as informações e integrar nos blocos já existentes<br>
• 9. Documentar<br> (***adicionar fluxograma***)

# Fluxograma de Hardware

• 1. Organizar os pré requisitos necessários<br>
• 2. Selecionar e adquirir os componentes de hardware necessários<br>
• Tela (televisão ou monitor)<br>
    • Placa de desenvolvimento (Raspberry Pi)<br>
    • Sensores<br>
    • Moldura IR (se necessário)<br>
• 3. Montar os componentes de hardware<br>
• 4. Integrar o hardware com o software<br>
• 5. Testar e aprimorar a funcionalidade do sistema integrado<br>
• 6. Documentar<br> (***adicionar fluxograma***)

# Desafios e Soluções

• Prototipagem iterativa para testar e aprimorar o produto ao longo do processo de desenvolvimento.<br>
• Substituição do sensor APDS9960 pelo PAJ7620U2 devido seu sensor melhor (***adicionar informações comparativas***).<br>
• Pelas pesquisas a Pi Camera parece ter uma capacidade de visão computacional bem maior que os módulos básicos de gestão de movimentos, onde juntamente com o módulo OpenCV ela consegue trackear até mesmo todas as juntas da mão, dando uma possibilidade bem maior no desenvolvimento de aplicabilidades para o espelho inteligente. Custo elevado (R$ 250,00).<br>
• * O uso do televisor é interessante pois já tem a saída de áudio, caso formos usar um monitor de computador precisamos usar uma caixa de som externa.<br>
• * O uso da moldura de madeira talvez não seja necessário dependendo do custo, mas teríamos que substituir por um vidro inteiro.<br>
• * O uso das API’s vai depender do que vamos querer implementar.<br>
• Durante a compra do APDS9960 foi identicado um erro que não o fazia funcionar, durante extensas pesquisas, soube-se que o sensor era de origem chinesa, dito isso, tivemos que modificar seu arquivo base (SparkFun_APDS9960.h), onde tivemos que modificar o hexadecimal da #define APDS9960_ID_1 0xAB, para #define APDS9960_ID_1 0xA8, fazendo o sensor funcionar corretamente. (***adicionar imagem***)<br>
• No raspberry pi os desafios são constantes, durante o desenvolvimento do módulo de sensor de movimentos, tivemos que habilitar a porta i2c e ajustar o baudrate do raspberry para a detecção correta do sensor, a partir desta etapa utilizando o comando (sudo i2cdetect -y 1) podemos ver agora que o sensor é visivel pelos pinos do raspberry pi. (***adicionar imagem***)<br>
• Também devido as documentações dos módulos estarem um pouco desatualizadas, foi preciso um esforço extenso analisando os códigos para refatorar as partes onde outrora estavam desatualizadas. Foram adicionados logs de mensagens no terminal para um debugger mais eficiente. (***adicionar imagem***)<br>
