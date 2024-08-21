## Teste funcionalidade da tampa de apoio
Foram feitos 2 prototipos sendo o primeiro em papelão ([Figura 1](#fig-tampa-em-papelao)) com o proposito de verificar encaixes e resistencia. E o segundo em madeira ([Figura 2](#fig-tampa-em-madeira)) para vibializar o teste de temperatura.

||
|:-:|
|![Tampa em Papelão](./assets/prototipo_tampa_v1.jpg)|
|<a id="fig-tampa-em-papelao">**Figura 1** - Tampa em Papelão </a>|

||
|:-:|
|![Tampa em Madeira](./assets/prototipo_tampa_v2.jpg)|
|<a id="fig-tampa-em-madeira">**Figura 2** - Tampa em Madeira </a>|

## Teste sensor de temperatura (DS18B20)

Foi realizado o teste do sensor utilizando o valor de tensão que haviamos projetado para o circuito, que até então era 3,3V, porém o sensor não funcionou nessa faixa de tensão. O teste foi repetido com 5V e assim foi possível estabelecer a comunicação com sucesso. Como alguns outros pontos do circuito também requisitam 5V decidimos trocar o microcontrolador e trabalhar com 5V em todo o circuito (exlcuindo o ebulidor, que funciona em 220 VAC, e os eletrodos, que utilizam 24 VDC) 

## Teste de transferencia de temperatura
Para o teste foi colocado 1 litro de água no becker externo e 500mL no interno. O ebulidor ficou no becker externo. Quando a temperatura da água no becker interno chegou a 60°C, a água no becker externo estava em 83°C.
Esse processo levou em torno de 7 minutos. Após desligado o ebulidor ambas as temperaturas elas se igualaram em torno de 70°C.

## Teste do motor DC (3V 13100 RPM DC MOTOR)

## Teste da corrente nos eletrodos
Para o teste foi colocado 500m de água desmineralizada no becker interno. A distância entre os eletrodos foi ajustada até a obtenção dos 10mA aproximados necessários para o processo de eletrólise continuo da prata em solução aquosa.

Esse processo levou em torno de 5 minutos. Sendo o momento inicial de ionização o mais demorado, pois após o primeiro momento com corrente na faixa de 10mA mesmo com a inversão dos pólos o tempo para estabilizar novamente em 10mA foi menor.
