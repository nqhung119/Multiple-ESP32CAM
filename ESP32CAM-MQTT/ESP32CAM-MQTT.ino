#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" 
#include "soc/rtc_cntl_reg.h"  
#include "esp_http_server.h"
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <SimpleTimer.h>
#include <base64.h>

const char* ssid = "QUANGHUNG";
const char* password = "11092002";
const char* mqttServer = "192.168.137.1";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* mqttClientName = "ESP32CAM";

#define FLASH_GPIO 4
#define ESP32CAM_PUBLISH_TOPIC   "esp32cam1"
const int bufferSize = 1024 * 23;

WiFiClient espClient;
PubSubClient client(espClient);
JSONVar data;
SimpleTimer timer;

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

#define PART_BOUNDARY "123456789000000000000987654321"

#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#else
  #error "Camera model not selected"
#endif

void grabImage() {
  camera_fb_t * fb = esp_camera_fb_get();
  if(fb != NULL) {
    if(fb->format == PIXFORMAT_JPEG && fb->len < bufferSize) {
      String buffer = base64::encode(fb->buf, fb->len);

      if (buffer.length() > 102400) {
        Serial.println("Image size too big");
        esp_camera_fb_return(fb);
        return;
      }

      Serial.print("Publishing...");
      if (client.publish(ESP32CAM_PUBLISH_TOPIC, buffer.c_str())) {
        Serial.println("Published");
      } else {
        Serial.println("Error");
      }
    } else {
      Serial.println("Image format not JPEG or size too big");
    }
    esp_camera_fb_return(fb);
  } else {
    Serial.println("Failed to get camera frame");
  }
  delay(1);
}


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
 
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  
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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    Serial.println("PSRAM found. Setting up camera with PSRAM");
    //config.frame_size = FRAMESIZE_HVGA;
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  } else {
    //config.frame_size = FRAMESIZE_HVGA;
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
  }
  
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  connectInternet();
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);

  pinMode(FLASH_GPIO, OUTPUT);
  digitalWrite(FLASH_GPIO, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timer.run();
  if(client.connected()) grabImage();
}
