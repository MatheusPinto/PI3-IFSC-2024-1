#define BLYNK_TEMPLATE_NAME "sensoresPiscinaPI"
#define BLYNK_AUTH_TOKEN "0oEwDgW0bMfT5bqdAGDEgFR1L_aLgamq"
#define BLYNK_TEMPLATE_ID "TMPL2pSGBuVWA"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "DHT.h"

#define DHTPIN 12
#define DHTTYPE DHT11 
#define BUZZER_PIN 14
#define MAGNITUDE_THRESCHOLD 4
#define MEASURE_COUNT 5
#define LED_PIN 13
#define LED_POSITIVE_PIN 15

DHT dht(DHTPIN, DHTTYPE);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "DESKTOP-ADKUULD 3652"; //Rede WiFi
char pass[] = "FORMATURA"; //Senha da rede WiFi

float magnitudes[MEASURE_COUNT];
int currentIndex = 0;
float total = 0;

Adafruit_MPU6050 mpu;
BlynkTimer timer;

void setup(void) {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  while (!Serial)
    delay(100);

  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(100);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_POSITIVE_PIN, OUTPUT);
  
  for (int i = 0; i < MEASURE_COUNT; i++) {
    magnitudes[i] = 0;
  }

  Serial.println("");
  delay(100);
}

void loop() {
  Blynk.run();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float magnitude = sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.y * a.acceleration.y);

  total -= magnitudes[currentIndex];
  magnitudes[currentIndex] = magnitude;
  total += magnitudes[currentIndex];
  currentIndex = (currentIndex + 1) % MEASURE_COUNT;

  double averageMagnitude = total / MEASURE_COUNT;

  if (averageMagnitude > MAGNITUDE_THRESCHOLD) {
     tone(BUZZER_PIN, 1000, 500);
     Blynk.virtualWrite(V3, 1);
  } else {
    noTone(BUZZER_PIN);
    Blynk.virtualWrite(V3, 0);
  }

  if (a.acceleration.z < 0) {
    digitalWrite(LED_PIN, HIGH); // Acende o LED
  } else {
    digitalWrite(LED_PIN, LOW);  // Apaga o LED
  }

    // Controle do segundo LED para aceleração positiva (Z > 0)
  if (a.acceleration.z > 0) {
    digitalWrite(LED_POSITIVE_PIN, HIGH); // Acende o LED
  } else {
    digitalWrite(LED_POSITIVE_PIN, LOW);  // Apaga o LED
  }


  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Agitacao da agua: ");
  Serial.print(averageMagnitude);

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" graus");

  float tempAtual = temp.temperature;
  float umidade = dht.readHumidity();

  Serial.print("Umidade: ");
  Serial.println(umidade);

  Blynk.virtualWrite(V0, tempAtual);
  Blynk.virtualWrite(V1, umidade);
  Blynk.virtualWrite(V4, averageMagnitude);

  Serial.println("");
  delay(500);
}