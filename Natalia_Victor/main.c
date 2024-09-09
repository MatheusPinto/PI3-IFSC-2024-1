/*
 * Analisador de Wi-Fi
 * Autores: Natália Besen e Victor Lompa
 *
 * Descrição:
 * Este código é implementado para o ESP32 e realiza mediçaõ do RSSI de redes Wi-Fi, calcula a latência,
 * e envia os dados para um servidor Python através de requisições HTTP.
 */


#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "ping/ping_sock.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/sys.h"
#include "driver/gpio.h"

// Defines para conexão Wi-Fi
#define WIFI_SSID 	""
#define WIFI_PASS 	""

// Defines para conexão em UDP
#define UDP_PORT 4210
#define BROADCAST_IP "255.255.255.255"
#define BROADCAST_MESSAGE "ESP32_HERE"
#define BROADCAST_CONFIRM "CONFIRMATION_RECEIVED";
#define TIMEOUT_SEC 2 // Tempo de timeout em segundos
#define MAX_ATTEMPTS 3

// Defines para cálculo de latência
#define pings 5

// Defines para configuração de GPIO
#define BUTTON_GPIO 26
#define LED_GPIO_G 25
#define LED_GPIO_Y 33
#define LED_GPIO_R 32
#define DEBOUNCE_DELAY 200 // Delay em milissegundos para o debounce


static const char *TAG = "[MAIN] Wi-Fi Analyzer";
static const char *target_SSID = WIFI_SSID;
static char post_data[1024];  // Buffer para armazenar os dados a serem enviados

uint32_t latency[pings];
uint32_t ping_count = 0;

int points_count = 0;
int points_value = 0; // Dado recebido pelo script python

static bool stop_broadcast = false;
static char python_ip[16] = {0}; // Para armazenar o IP do Python

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

static QueueHandle_t gpio_evt_queue = NULL;
static bool button_pressed = false; // Flag para indicar se o botão foi pressionado
static bool is_measuring = false; // Flag para indicar se uma medição está em andamento

// Funções de inicialização, configuração e manipulação
static void flash_init(void);
static void wifi_init(void);
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void ping_success_callback(esp_ping_handle_t hdl, void *args);
static void check_latency(void);
static void wifi_scan(void);
static void http_send_data(int end);
void scan_and_send_task(void *pvParameters);
void udp_broadcast_task(void *pvParameters);
void button_task(void *pvParameters);


static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		gpio_set_level(LED_GPIO_Y, 1); // Acende LED amarelo durante o processo de conexão{
		esp_wifi_connect();
	}

	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		ESP_LOGI(__func__, "Tentando reconexão.");
		gpio_set_level(LED_GPIO_Y, 0); // Apaga o LED amarelo
		gpio_set_level(LED_GPIO_R, 1); // Acende LED vermelho em caso de falha
		esp_wifi_connect();
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
	}

	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI(__func__, "Endereço IP obtido: " IPSTR, IP2STR(&event->ip_info.ip));
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
		gpio_set_level(LED_GPIO_Y, 0); // Apaga o LED amarelo após obter IP
		gpio_set_level(LED_GPIO_R, 0); // Acende LED vermelho em caso de falha

	}
}

/* Inicializa Memoria Flash */
static void flash_init(void)
{
	// Initialize NVS (Non-Volatile Storage) flash
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
}

/* Função para inicializacao da conexao Wi-Fi */
static void wifi_init(void)
{
	wifi_event_group = xEventGroupCreate();

    // Initialize WiFi
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
		.sta = {
			.ssid = WIFI_SSID,
			.password = WIFI_PASS,
		},
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi STA iniciado. Conectando ao SSID: %s", WIFI_SSID);
}

/* Callback para sucesso de ping */
static void ping_success_callback(esp_ping_handle_t hdl, void *args)
{
    uint32_t elapsed_time;
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    ESP_LOGI("check_latency", "Ping: %" PRIu32 " ms", elapsed_time);

    if(ping_count < pings){
    	latency[ping_count] = elapsed_time;
    	ping_count ++;
    }
}

/* Função para medir a latencia usando ping */
static void check_latency(void)
{
	uint32_t medLatency = 0;
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    config.count = pings;

    ip_addr_t target_addr;
    inet_pton(AF_INET, "8.8.8.8", &target_addr.u_addr.ip4);
    target_addr.type = IPADDR_TYPE_V4;
    config.target_addr = target_addr;

    esp_ping_callbacks_t cbs;
    memset(&cbs, 0, sizeof(cbs));
    cbs.on_ping_success = ping_success_callback;
    cbs.on_ping_end = NULL;  // Evita cálculo duplicado na função de finalização

    esp_ping_handle_t ping;
    ESP_ERROR_CHECK(esp_ping_new_session(&config, &cbs, &ping));
    esp_ping_start(ping);

    // Espera até que o ping termine antes de continuar
	vTaskDelay(pdMS_TO_TICKS(pings * 1000));  // Espera até que o número de pings termine (aproximadamente 1 segundo por ping)

	esp_ping_stop(ping);
    esp_ping_delete_session(ping);

    ping_count = 0;
    for(int i = 0; i < pings; i++) medLatency += latency[i];
    medLatency = medLatency/pings;

    ESP_LOGI(__func__, "Latency: %" PRIu32, medLatency);

    //strcpy(post_data, "");  // Inicializa o buffer de dados
    char ap_data[32];
    snprintf(ap_data, sizeof(ap_data), "%" PRIu32, medLatency);
	strcat(post_data, ap_data);
}

/* Função para varredura de redes Wi-Fi e criação dos dados a serem enviados */
static void wifi_scan(void)
{
	wifi_scan_config_t scan_config = {
		.ssid = (uint8_t *)target_SSID,
		.bssid = 0,
		.channel = 0,
		.show_hidden = true,
		.scan_type = WIFI_SCAN_TYPE_ACTIVE,
		.scan_time.active = {
			.min = 100,
			.max = 1000
		}
	};

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    uint16_t ap_count = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(__func__, "Total APs scanned = %" PRIu16, ap_count);

    if (ap_count == 0) {
    	ESP_LOGI(__func__, "Varredura Wi-Fi não encontrou SSID solicitado.");
    	return;
    }

    wifi_ap_record_t ap_info[ap_count];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_info));

    // Inicializa o buffer de dados
	strcpy(post_data, "");
	char ssid_data[46];
	snprintf(ssid_data, sizeof(ssid_data), "SSID:%s,%d\n", ap_info[0].ssid, points_count);
	strcat(post_data, ssid_data);

	for (int i = 0; i < ap_count; i++) {
		char ap_data[128];
		snprintf(ap_data, sizeof(ap_data),
				 "%02x-%02x-%02x-%02x-%02x-%02x,%d,%d\n",
				 ap_info[i].bssid[0], ap_info[i].bssid[1], ap_info[i].bssid[2],
				 ap_info[i].bssid[3], ap_info[i].bssid[4], ap_info[i].bssid[5],
				 ap_info[i].primary, ap_info[i].rssi);
		strcat(post_data, ap_data);
	}
    ESP_LOGI(__func__, "Dados da varredura Wi-Fi preparados para envio.");
}

/* Função para enviar a mensagem HTTP */
static void http_send_data(int end)
{
	char url[256];
	snprintf(url, sizeof(url), "http://%s:%d%s", python_ip, 5000, "/data");

	esp_http_client_config_t config = {
			.url = url,
			.method = HTTP_METHOD_POST,
			.event_handler = NULL,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_post_field(client, post_data, strlen(post_data));

	esp_err_t err;
	for (int i = 0; i < 3; i++) { // Tenta enviar até 3 vezes
		err = esp_http_client_perform(client);
		if (err == ESP_OK) {
			ESP_LOGI(__func__, "Dados enviados com sucesso");
			break;
		} else {
			ESP_LOGE(__func__, "Erro durante o envio dos dados: %s", esp_err_to_name(err));
			vTaskDelay(pdMS_TO_TICKS(1000)); // Aguardar 1 segundo antes de tentar novamente
		}
	}
    esp_http_client_cleanup(client);


    // Fecha o cliente HTTP se for o final
    if (end == 1) {
        ESP_LOGI(__func__, "Todos os pontos enviados.");
    }
}


void scan_and_send_task(void *pvParameters) {
    while (points_count < points_value) {
    	//Aguarda o botão ser pressionado e não estar executando uma medição
        if (button_pressed && !is_measuring) {
            button_pressed = false; // Reseta o estado do botão
            is_measuring = true; // Define que uma medição está em andamento
            gpio_set_level(LED_GPIO_Y, 0); // Apaga o LED amarelo ao iniciar a medição

            wifi_scan();
            check_latency();
            http_send_data(0); // Envia dados após a varredura
            points_count++;

            // Acende o LED verde para indicar que a medição foi concluída
            gpio_set_level(LED_GPIO_G, 1);

            // Aguardar alguns segundos para indicar conclusão e então apagar o LED
            vTaskDelay(pdMS_TO_TICKS(2000));
            gpio_set_level(LED_GPIO_G, 0);
            gpio_set_level(LED_GPIO_Y, 1); // Acende o LED amarelo enquanto aguarda a próxima pressão do botão

            is_measuring = false; // Medição concluída, libera para a próxima
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Finaliza o envio dos dados
    if (strcmp(post_data, "ALL_POINTS_SENT") != 0) {
        strcpy(post_data, "ALL_POINTS_SENT");
        http_send_data(1);
    }

    // Deixa o LED aceso para indicar que as medições foram concluídas
    gpio_set_level(LED_GPIO_G, 1);
    gpio_set_level(LED_GPIO_Y, 0);
    gpio_set_level(LED_GPIO_R, 0);


    // Encerrar a tarefa
    vTaskDelete(NULL);
}


//Envia mensagem de broadcast com IP e aguarda receber o IP do servidor
void udp_broadcast_task(void *pvParameters) {
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(UDP_PORT);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Erro ao criar socket: %d", errno);
        // Acende o LED vermelho para indicar erro
        gpio_set_level(LED_GPIO_R, 1);
        vTaskDelete(NULL);
        return;
    }

    // Permitir broadcast
    int broadcast_enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        ESP_LOGE(TAG, "Erro ao configurar socket em modo broadcast: %d", errno);
        // Acende o LED vermelho para indicar erro
        gpio_set_level(LED_GPIO_R, 1);
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    // Bind para a porta e permitir escutar mensagens de broadcast
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY; // Escuta em todas as interfaces
    local_addr.sin_port = htons(UDP_PORT);

    if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        ESP_LOGE(TAG, "Erro ao bindar socket: %d", errno);
        // Acende o LED vermelho para indicar erro
        gpio_set_level(LED_GPIO_R, 1);
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in source_addr;
    socklen_t socklen = sizeof(source_addr);
    char rx_buffer[128];

    // Configura o timeout para o socket
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        ESP_LOGE(TAG, "Erro ao configurar timeout do socket: %d", errno);
    }

    bool ip_received = false;
	int attempts = 0;
	const int max_attempts = MAX_ATTEMPTS;

	while (!stop_broadcast && attempts < max_attempts) {
		// Enviar mensagem de broadcast
		int err = sendto(sock, BROADCAST_MESSAGE, strlen(BROADCAST_MESSAGE), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

		if (err < 0) ESP_LOGE(TAG, "Erro no envio: %d", errno);
		else ESP_LOGI(TAG, "Mensagem broadcast enviada");

        // Verificar se há resposta
        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
        if (len > 0) {
            rx_buffer[len] = '\0';  // Null-terminate the received data
            ESP_LOGI(TAG, "Resposta recebida.");
            // Apaga o LED vermelho
            gpio_set_level(LED_GPIO_R, 0);

            // Verificar se a mensagem começa com "PYTHON_IP:"
            if (strncmp(rx_buffer, "PYTHON_IP:", 10) == 0) {
                // Extrair o IP do Python e o valor de pontos
                char *token = strtok(rx_buffer + 10, ","); // Salta "PYTHON_IP:" e começa a dividir
                if (token != NULL) {
                    strncpy(python_ip, token, sizeof(python_ip) - 1);
                    python_ip[sizeof(python_ip) - 1] = '\0'; // Null-terminate
                }

                token = strtok(NULL, ":");
                if (token != NULL && strcmp(token, "POINTS") == 0) {
                    token = strtok(NULL, "\0"); // Pega o valor após "POINTS:"
                    if (token != NULL) {
                        points_value = atoi(token); // Converte o valor para inteiro
                    }
                }

                ESP_LOGI(TAG, "IP python: %s, Número de pontos: %d", python_ip, points_value);

                // Confirmar o recebimento do IP com o servidor
                const char *confirmation_message = BROADCAST_CONFIRM
				int err = sendto(sock, confirmation_message, strlen(confirmation_message), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

                if (err < 0) ESP_LOGE(TAG, "Erro ao enviar confirmação: %d", errno);
				else ESP_LOGI(TAG, "Mensagem de confirmação enviada");

                ip_received = true;
                stop_broadcast = true;

            } else {
                ESP_LOGI(TAG, "Formato da resposta é inválido.");
            }

        } else {
            ESP_LOGI(TAG, "Sem resposta, tentando novamente.");
            // Acende o LED vermelho para indicar erro
            gpio_set_level(LED_GPIO_R, 1);
            attempts++;
        }

        vTaskDelay(TIMEOUT_SEC * 1000 / portTICK_PERIOD_MS); // Aguardar *timeout* segundos antes de reenviar
    }

	if (!ip_received) {
		ESP_LOGE(TAG, "Falha ao receber o IP do servidor após %d tentativas", max_attempts);
		// Acende o LED vermelho para indicar erro
		gpio_set_level(LED_GPIO_R, 1);
	}

    if (sock != -1) {
        ESP_LOGI(TAG, "Encerrando socket");
        shutdown(sock, 0);
        close(sock);
    }

    gpio_set_level(LED_GPIO_Y, 1); // Acende o LED amarelo
    xTaskCreate(button_task, "button_task", 4096, NULL, 10, NULL);
    vTaskDelete(NULL);
}

/* Manipulador da interrupção do GPIO */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t io_num = (uint32_t) arg;
    if (!is_measuring && !button_pressed)
    	xQueueSendFromISR(gpio_evt_queue, &io_num, NULL);
}


//Realiza o debouncer do botão e seta uma varíavel com true quando botão é pressionado
void button_task(void *pvParameters) {
    uint32_t io_num;
    static uint32_t last_press_time = 0;

    while (1) {
        // Espera por um evento na fila de interrupção do GPIO
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

            // Verifica se a interrupção foi causada pelo botão
            if (io_num == BUTTON_GPIO) {
                // Verifica se o botão foi pressionado após o período de debounce
                if ((current_time - last_press_time) > DEBOUNCE_DELAY) {
                    if (!button_pressed) {
                        button_pressed = true;  // Marcar que o botão foi pressionado
                        ESP_LOGI(TAG, "Botão pressionado");

                        // Iniciar a tarefa de medição e envio
                        xTaskCreate(&scan_and_send_task, "scan_and_send_task", 4096, NULL, 5, NULL);
                    }
                    last_press_time = current_time;  // Atualiza o tempo da última pressão
                }
            }
        }
    }
}

/* Função para configurar o GPIO do botão */
static void button_init(void)
{
    gpio_config_t io_conf;
    // Configura GPIO para entrada
    io_conf.intr_type = GPIO_INTR_ANYEDGE;  // Interrupção na borda de subida ou descida
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // Cria a fila para o evento do GPIO
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // Instala a rotina de serviço de GPIO
    gpio_install_isr_service(0);
    // Adiciona o manipulador da interrupção do GPIO
    gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, (void*) BUTTON_GPIO);
}


void app_main(void)
{
	flash_init();
    wifi_init();
    button_init();
    gpio_set_direction(LED_GPIO_G, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GPIO_Y, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GPIO_R, GPIO_MODE_OUTPUT);

       /* Aguarda até que a conexão seja estabelecida (bit CONNECTED_BIT definido pelo evento do Wi-Fi) */
       EventBits_t bits = xEventGroupWaitBits(
           wifi_event_group,
           CONNECTED_BIT,
           pdFALSE,
           pdTRUE,
           portMAX_DELAY);

       if (bits & CONNECTED_BIT) {
           ESP_LOGI(TAG, "Conectado ao AP, endereço IP obtido. Pronto para varredura e envio de dados...");
           xTaskCreate(udp_broadcast_task, "udp_broadcast_task", 4096, NULL, 5, NULL);
       } else {
           ESP_LOGE(TAG, "Falha ao conectar ao AP");
       }

}
