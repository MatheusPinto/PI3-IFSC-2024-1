# Operação

Nesta seção está disponível videos para apresentação dos resultados da maquete.

Clique no link abaixo para assistir o vídeo:
<br> https://drive.google.com/drive/folders/1OMNfv0nzgsu3z_s7YsmZOHQiT3k73YoR?usp=sharing


## Nesta seção, apresentamos os resultados do protótipo do limpador de piscinas automatizado.
Como podemos verificar, o objetivo principal do projeto foi alcançado. A integração de diversas tecnologias, como o ESP8266, sensores de aceleração (MPU6050) e de temperatura/umidade (DHT11), foi concluída com sucesso. O sistema de monitoramento da superfície da piscina está funcionando, e os dados de temperatura, umidade e agitação da água são exibidos corretamente na plataforma Blynk.

## Cálculo da Magnitude e sua Importância
No projeto, a magnitude da aceleração é calculada utilizando os valores obtidos pelo sensor MPU6050, que mede a aceleração nos eixos X, Y e Z. A fórmula usada para calcular a magnitude da aceleração é:

Onde:
- `a_x`, `a_y` e `a_z` são os valores de aceleração nos respectivos eixos X, Y e Z.

Essa fórmula é crucial, pois a magnitude nos fornece uma medida do quão intensa é a movimentação ou agitação da superfície da água da piscina. No código, a magnitude calculada é comparada com um valor limite (threshold) para determinar se o nível de agitação é suficiente para acionar o sistema de alerta (buzzer e LED).

Essa informação é essencial porque permite que o sistema identifique quando há movimentos anormais na piscina, como quedas de objetos ou excesso de detritos, possibilitando uma resposta rápida e automática. Além disso, o cálculo correto da magnitude garante que o sistema seja confiável e preciso ao detectar alterações significativas, evitando falsos alarmes.

Esse cálculo de magnitude, junto com a configuração de threshold adequada, garante que apenas mudanças importantes no comportamento da água acionem o alarme, melhorando a eficiência do sistema.









## Erros e Problemáticas
Como todo protótipo, mesmo com o objetivo principal alcançado, o sistema ainda apresenta alguns problemas secundários. Esses erros, apesar de não comprometerem o funcionamento principal, são pontos a serem considerados para melhorias futuras. O primeiro problema é a sensibilidade do acelerômetro, que às vezes gera leituras incorretas devido a pequenas vibrações no ambiente. 
O segundo está relacionado à instabilidade da conexão WiFi, que ocasiona a desconexão temporária do sistema e pode gerar falhas no envio de dados para o Blynk. Esses problemas são oportunidades para futuras pesquisas e ajustes numa encubadora fora do IFSC, visando otimizar a precisão e a confiabilidade do sistema.
