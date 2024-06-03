#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <SimpleTimer.h>

const char* ssid = "QUANGHUNG";
const char* password = "11092002";
const char* mqttServer = "192.168.137.1";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* mqttClientName = "ESP32Control";
const char* mqttTopic = "control";

// Motor A
int enA = 16;
int in1 = 17;
int in2 = 18;
// Motor B
int enB = 19;
int in3 = 21;
int in4 = 22;

int speed = 0;
int direction = 1;

WiFiClient espClient;
PubSubClient client(espClient);
JSONVar data;
SimpleTimer timer;

bool turning = false;
unsigned long lastTurnTime = 0;
int turnAngle = 0;

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
      if (direction == 1) {
        tangtoc();
      }
      else if (direction == 0) {
        giamtoc();
      }
      Serial.print("Da tang toc.");
    } else if (messageTemp == "sangtrai") {
      sangtrai();
      Serial.print("Da sang trai.");
    } else if (messageTemp == "sangphai") {
      sangphai();
      Serial.print("Da sang phai.");
    } else if (messageTemp == "giamtoc") {
      if (direction == 1) {
        giamtoc();
      }
      else if (direction == 0) {
        tangtoc();
      }
      Serial.print("Da giam toc.");
    } else if (messageTemp == "phanh") {
      phanh();
      Serial.print("Da phanh.");
    }
  }
  else if (String(topic) == "directioncontrol") {
    Serial.print("Da nhan thong tin chuyen huong:");
    if (messageTemp == "tien") {
      direction = 1;
      Serial.print("Huong di chuyen lien len phia truoc.");
    } else if (messageTemp == "lui") {
      direction = 0;
      Serial.print("Huong di chuyen lui lai phia sau.");
    }
  }
}

void tangtoc() {
  if (speed < 255) {
    speed = speed + 50;
  } else {
    speed = 0;
  }

  analogWrite(enA, speed);
  analogWrite(enB, speed);

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  delay(100);
}

void giamtoc() {
  if (speed > 0) {
    speed = speed - 50;
  } else {
    speed = 0;
  }

  analogWrite(enA, speed);
  analogWrite(enB, speed);

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  delay(100);
}

void sangphai() {
  if (!turning) {
    phanh();
    turning = true;
    lastTurnTime = millis();
    turnAngle = 0;
  } else {
    if (millis() - lastTurnTime < 1000) {
      phanh();
    } else if (turnAngle < 30) {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      turnAngle++;
    } else {
      phanh();
      turning = false;
    }
  }
}

void sangtrai() {
  if (!turning) {
    phanh();
    turning = true;
    lastTurnTime = millis();
    turnAngle = 0;
  } else {
    if (millis() - lastTurnTime < 1000) {
      phanh();
    } else if (turnAngle < 30) {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      turnAngle++;
    } else {
      phanh();
      turning = false;
    }
  }
}

void phanh() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void setup() {
  connectInternet();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.begin(115200);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timer.run();
}
