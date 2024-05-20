#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <SimpleTimer.h>

// Motor A
int motor1Pin1 = 19; 
int motor1Pin2 = 21; 
int enable1Pin = 18; 

// Setting minimum duty cycle
int dutyCycle = 0;

const char* ssid = "QUANGHUNG";
const char* password = "11092002";
const char* mqttServer = "192.168.137.1";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* mqttClientName = "ESP32Control";
const char* mqttTopic = "control";

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
      client.subscribe(mqttTopic);
    } else {
      Serial.println("Try again.");
    }
  }
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

  if (String(topic) == "control") {
    Serial.print("Da nhan thong tin dieu khien:");
    if (messageTemp == "tangtoc") {
      tangtoc();
      Serial.print("Da tang toc.");
    } else if (messageTemp == "sangtrai") {
      sangtrai();
      Serial.print("Da sang trai.");
    }
    else if (messageTemp == "sangphai") {
      sangphai();
      Serial.print("Da sang phai.");
    }
     else if (messageTemp == "giamtoc") {
      giamtoc();
      Serial.print("Da giam toc.");
    }
    else if (messageTemp == "phanh") {
      phanh();
      Serial.print("Da phanh.");
    }
  }
}

void tangtoc() {

}

void sangtrai() {

}

void sangphai() {
  
}

void giamtoc() {

}

void phanh() {

}

void setup() {
  connectInternet();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  Serial.begin(115200);

  // testing
  Serial.print("Testing DC Motor...");
}

void loop() {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timer.run();

  //Apply power to spin at maximum speed
  digitalWrite(enable1Pin, HIGH);

  // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  delay(1000);

  // Move DC motor backwards at maximum speed
  Serial.println("Moving Backwards");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW); 
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  delay(1000);

  // Move DC motor forward with increasing speed
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  while (dutyCycle <= 255){
    analogWrite(enable1Pin, dutyCycle);   
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 5;
    delay(500);
  }
  dutyCycle = 60;
}