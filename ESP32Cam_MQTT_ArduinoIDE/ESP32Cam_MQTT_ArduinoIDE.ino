#include "Arduino.h"
#include "esp_camera.h"
#include <base64.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define CAMERA_MODEL_AI_THINKER  // Has PSRAM

#include "camera_pins.h"

// AI Thinker Model - LED Driving Pins
#define ESP32CAM_LED_INBUILT 33
#define ESP32CAM_LED_FLASH 4

const char* ssid = "QUANGHUNG";
const char* password = "11092002";
const char* mqttServer = "192.168.137.1";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* mqttClientName = "ESP32CAM";
#define ESP32CAM_PUBLISH_TOPIC   "esp32cam0"

WiFiClient espClient;
PubSubClient client(espClient);

void connectInternet() {
  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi Wi-Fi.");
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    delay(1000);
    Serial.print(".");
    retries++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Khong the ket noi Wi-Fi.");
  } else {
    Serial.println("Da ket noi Wi-Fi.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "ESP32Client";

    if (client.connect(mqttClientName, mqttUser, mqttPassword)) {
      Serial.println("Connected.");
    } else {
      Serial.println("Try again.");
    }
  }
}

long lastMsg = 0;
// char msg[50];
// int value = 0;

void setup() {
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)

  Serial.setDebugOutput(true);

  // FLASH LED
  pinMode(ESP32CAM_LED_FLASH, OUTPUT);
  digitalWrite(ESP32CAM_LED_FLASH, LOW);

  // buffer.reserve(32000);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;  // was at 20
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;  //800 x 600 necessary for Adafruit IO
  config.jpeg_quality = 30;
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  connectInternet();
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000) {

    // Capture picture
    digitalWrite(ESP32CAM_LED_FLASH, HIGH);
    camera_fb_t* fb = NULL;
    fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    } else {
      Serial.println("Camera Captured");
    }
    digitalWrite(ESP32CAM_LED_FLASH, LOW);
    // delay(1000);

    // size_t size = fb->len;
    //String buffer = base64::encode((uint8_t*)fb->buf, fb->len);
    String buffer = base64::encode(fb->buf, fb->len);
    Serial.println(buffer);

    unsigned int length();
    Serial.println("Buffer Length: ");
    Serial.print(buffer.length());
    Serial.println("");

    if (buffer.length() > 102400) {
      Serial.println("Image size too big");
      return;
    }

    Serial.print("Publishing...");
    if (client.publish(ESP32CAM_PUBLISH_TOPIC, buffer.c_str())) {
      Serial.print("Published");
    } else {
      Serial.println("Error");
    }
    Serial.println("");

    lastMsg = now;
  }
}
