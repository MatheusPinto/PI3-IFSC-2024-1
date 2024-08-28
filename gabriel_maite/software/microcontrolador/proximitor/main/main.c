#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_mac.h"
#include <driver/i2c.h>
#include <driver/i2c_master.h>

#include "../components/ADS1115/ADS1115.h"

#define STEP_MOTOR_GPIO_EN 15
#define STEP_MOTOR_GPIO_DIR 2
#define STEP_MOTOR_GPIO_STEP 4
#define STEP_MOTOR_ENABLE_LEVEL 0 // DRV8825 is enabled on low level
#define STEP_MOTOR_SPIN_DIR_CLOCKWISE 0
#define STEP_MOTOR_SPIN_DIR_COUNTERCLOCKWISE !STEP_MOTOR_SPIN_DIR_CLOCKWISE

#define PWM_FREQUENCY 200 // 200 Hz
#define PWM_RESOLUTION LEDC_TIMER_8_BIT
#define PWM_MAX_DUTY_CYCLE ((1 << PWM_RESOLUTION) - 1) // 255 for 8-bit resolution

#define NUM_ROTATIONS 12

#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "PROXIMITOR"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA /*Choose show mode: show data or speed*/

#define SDA_IO (21) /*!< gpio number for I2C master data  */
#define SCL_IO (22) /*!< gpio number for I2C master clock */

#define FREQ_HZ (100000)   /*!< I2C master clock frequency */
#define TX_BUF_DISABLE (0) /*!< I2C master doesn't need buffer */
#define RX_BUF_DISABLE (0) /*!< I2C master doesn't need buffer */

#define I2C_NUM I2C_NUM_0               /*!< I2C number */
#define I2C_MODE I2C_MODE_MASTER        /*!< I2C mode to act as */
#define I2C_RX_BUF_STATE RX_BUF_DISABLE /*!< I2C set rx buffer status */
#define I2C_TX_BUF_STATE TX_BUF_DISABLE /*!< I2C set tx buffer status */
#define I2C_INTR_ALOC_FLAG (0)          /*!< I2C set interrupt allocation flag */

/* i2c setup ----------------------------------------- */
// Config profile for espressif I2C lib
i2c_config_t i2c_cfg = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = SDA_IO,
    .scl_io_num = SCL_IO,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .master.clk_speed = FREQ_HZ,
};

/* ADS1115 setup ------------------------------------- */
// Below uses the default values speficied by the datasheet
ads1115_t ads1115_cfg = {
    .reg_cfg = ADS1115_CFG_LS_COMP_MODE_TRAD | // Comparator is traditional
               ADS1115_CFG_LS_COMP_LAT_NON |   // Comparator is non-latching
               ADS1115_CFG_LS_COMP_POL_LOW |   // Alert is active low
               ADS1115_CFG_LS_COMP_QUE_DIS |   // Compator is disabled
               ADS1115_CFG_LS_DR_1600SPS |     // No. of samples to take
               ADS1115_CFG_MS_PGA_FSR_4_096V | // gain 1
               ADS1115_CFG_MS_MODE_SS,         // Mode is set to single-shot
    .dev_addr = 0x48,
};

const char *tag = "Bluetooth";
const char *TaG = "ADS1115";
const char *TAG = "MOTOR";

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

static void initADS1115(void)
{

    ESP_LOGI(TaG, "Starting ADS1115");

    // Setup I2C
    i2c_param_config(I2C_NUM, &i2c_cfg);
    i2c_driver_install(I2C_NUM, I2C_MODE, I2C_RX_BUF_STATE, I2C_TX_BUF_STATE, I2C_INTR_ALOC_FLAG);

    // Setup ADS1115
    ADS1115_initiate(&ads1115_cfg);
}

static void initMotor(void)
{
    ESP_LOGI(TAG, "Initialize EN + DIR GPIO");
    gpio_config_t en_dir_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1ULL << STEP_MOTOR_GPIO_DIR) | (1ULL << STEP_MOTOR_GPIO_EN),
    };
    ESP_ERROR_CHECK(gpio_config(&en_dir_gpio_config));

    // Initialize PWM for motor control
    ESP_LOGI(TAG, "Initialize PWM");

    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t channel_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = STEP_MOTOR_GPIO_STEP,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));

    ESP_LOGI(TAG, "Set spin direction");
    gpio_set_level(STEP_MOTOR_GPIO_DIR, STEP_MOTOR_SPIN_DIR_CLOCKWISE);
    ESP_LOGI(TAG, "Enable step motor");
    gpio_set_level(STEP_MOTOR_GPIO_EN, STEP_MOTOR_ENABLE_LEVEL);
}

float readSensorData()
{
    uint16_t result = 0;

    // Requisita dado no canal A0
    ADS1115_request_single_ended_AIN0();      

    // Checa o estado da conversão - retorna True se completa
    while(!ADS1115_get_conversion_state()) 
      vTaskDelay(pdMS_TO_TICKS(5));          
    
    // Retorna o última valor de conversão
    result = ADS1115_get_conversion();

    ESP_LOGI(TaG,"Conversion Value: %d", result); 

    float data = (result / (float)65536) * 8.192 * 5.84 * 2;

    ESP_LOGI(TaG,"Conversão: %.2f", data);

    return data;
}

char *formatMessage(float *sensor_data, int num_rotations)
{
    // Define o tamanho máximo da mensagem
    static char message[256]; // Buffer para armazenar a string resultante

    // Inicializa a string de mensagem
    message[0] = '\0';

    // Adiciona o valor inicial (abre colchetes) à string
    strcat(message, "[");

    // Itera pelos valores do vetor e os adiciona à string
    for (int i = 0; i < num_rotations; i++)
    {
        char buffer[32];                                          // Buffer temporário para cada número
        snprintf(buffer, sizeof(buffer), "%.3f", sensor_data[i]); // Converte o float para string
        strcat(message, buffer);                                  // Adiciona o número convertido à mensagem

        // Adiciona uma vírgula e espaço entre os números, exceto após o último
        if (i < num_rotations - 1)
        {
            strcat(message, ", ");
        }
    }

    // Adiciona o valor final (fecha colchetes) à string
    strcat(message, "]");

    // Retorna a string formatada
    return message;
}

float *rotateMotorAndCollectData(void)
{
    float *sensor_data = malloc(NUM_ROTATIONS * sizeof(float));
    uint32_t duty_cycle = PWM_MAX_DUTY_CYCLE / 2; // 50% de ciclo de trabalho

    if (sensor_data == NULL)
    {
        ESP_LOGE(TAG, "Memory allocation failed for sensor_data");
        return NULL;
    }

    for (int i = 0; i < NUM_ROTATIONS; i++)
    {

        // Gira o motor no sentido horário
        ESP_LOGI(TAG, "Girando o motor no sentido horário");
        gpio_set_level(STEP_MOTOR_GPIO_DIR, STEP_MOTOR_SPIN_DIR_CLOCKWISE);
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty_cycle));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0));

        vTaskDelay(pdMS_TO_TICKS(1000)); // Gira por 1 segundo

        // Para o motor
        ESP_LOGI(TAG, "Parando o motor");
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0));

        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera por 1 segundo antes de ler o sensor

        sensor_data[i] = readSensorData(); // Leitura dos dados do sensor
    }

    return sensor_data;
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)
    {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(tag, "ESP_SPP_INIT_EVT");
        esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(tag, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(tag, "ESP_SPP_OPEN_EVT");
        {
            const char *message = "Hello, Bluetooth!";
            esp_spp_write(param->open.handle, strlen(message), (uint8_t *)message);
            ESP_LOGI(tag, "Message sent: %s", message);
        }
        break;
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(tag, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(tag, "ESP_SPP_START_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(tag, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(tag, "ESP_SPP_DATA_IND_EVT len=%d handle=%d",
                 param->data_ind.len, param->data_ind.handle);
        esp_log_buffer_hex("", param->data_ind.data, param->data_ind.len);

        char received_data[5];
        snprintf(received_data, sizeof(received_data), "%s", param->data_ind.data);

        const char *message = "";
        if (strncmp(received_data, "ON", 2) == 0)
        {
            float *sensor_data = rotateMotorAndCollectData();
            message = formatMessage(sensor_data, NUM_ROTATIONS);
            ESP_LOGI(TAG, "Message: %s", message);
            free(sensor_data);
        }
        if (strcmp(message, "") != 0)
        {   
            esp_spp_write(param->data_ind.handle, strlen(message), (uint8_t *)message);
            ESP_LOGI(tag, "Message sent: %s", message);
        }
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(tag, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(tag, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(tag, "ESP_SPP_SRV_OPEN_EVT");
        break;
    case ESP_SPP_SRV_STOP_EVT:
        ESP_LOGI(tag, "ESP_SPP_SRV_STOP_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(tag, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_BT_GAP_AUTH_CMPL_EVT:
    {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGI(tag, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(tag, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        }
        else
        {
            ESP_LOGE(tag, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:
    {
        ESP_LOGI(tag, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit)
        {
            ESP_LOGI(tag, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        }
        else
        {
            ESP_LOGI(tag, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(tag, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(tag, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(tag, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(tag, "ESP_BT_GAP_MODE_CHG_EVT mode:%d", param->mode_chg.mode);
        break;

    default:
    {
        ESP_LOGI(tag, "event: %d", event);
        break;
    }
    }
    return;
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK)
    {
        ESP_LOGE(tag, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK)
    {
        ESP_LOGE(tag, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);

    initADS1115();
    initMotor();
}
