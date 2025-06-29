#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

// ==== Pin Definitions for ESP32 Devkit V1 ====
// ==== Pin Definitions for ESP32 Devkit V1 (Safe & Available) ====
#define DHT1_PIN 4
#define FAN1_PIN 23
#define COOLER1_PIN 21
#define HUMIDIFIER1_PIN 18
#define PUMP1_PIN 27

#define DHT2_PIN 5
#define FAN2_PIN 19
#define COOLER2_PIN 22
#define HUMIDIFIER2_PIN 14
#define PUMP2_PIN 13

#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define DHTTYPE DHT11
DHT dht1(DHT1_PIN, DHTTYPE);
DHT dht2(DHT2_PIN, DHTTYPE);

AsyncWebServer server(80);

bool isManualMode = false;
int fan1Speed = 128;
int fan2Speed = 128;
bool cooler1State = false, cooler2State = false;
bool humidifier1State = false, humidifier2State = false;
bool pump1State = false, pump2State = false;

void autoControl(float temp1, float hum1, float temp2, float hum2) {
  Serial.println("Auto mode control active");
  Serial.print("Temp1: "); Serial.print(temp1); Serial.print(" C, Hum1: "); Serial.print(hum1); Serial.println(" %");
  Serial.print("Temp2: "); Serial.print(temp2); Serial.print(" C, Hum2: "); Serial.print(hum2); Serial.println(" %");

  // Greenhouse 1
  if (temp1 > 28.0) {
    digitalWrite(COOLER1_PIN, RELAY_ON);
    analogWrite(FAN1_PIN, fan1Speed);
    Serial.print("Greenhouse 1: Fan Speed: "); Serial.println(fan1Speed);
    Serial.println("Greenhouse 1: Cooler ON, Fan ON");
  } else {
    digitalWrite(COOLER1_PIN, RELAY_OFF);
    analogWrite(FAN1_PIN, 0);
    Serial.println("Greenhouse 1: Cooler OFF, Fan OFF");
  }
  digitalWrite(HUMIDIFIER1_PIN, hum1 < 70 ? RELAY_ON : RELAY_OFF);
  Serial.print("Greenhouse 1: Humidifier "); Serial.println(hum1 < 70 ? "ON" : "OFF");

  // Greenhouse 2
  if (temp2 > 28.0) {
    digitalWrite(COOLER2_PIN, RELAY_ON);
    analogWrite(FAN2_PIN, fan2Speed);
    Serial.print("Greenhouse 2: Fan Speed: "); Serial.println(fan2Speed);
    Serial.println("Greenhouse 2: Cooler ON, Fan ON");
  } else {
    digitalWrite(COOLER2_PIN, RELAY_OFF);
    analogWrite(FAN2_PIN, 0);
    Serial.println("Greenhouse 2: Cooler OFF, Fan OFF");
  }
  digitalWrite(HUMIDIFIER2_PIN, hum2 < 70 ? RELAY_ON : RELAY_OFF);
  Serial.print("Greenhouse 2: Humidifier "); Serial.println(hum2 < 70 ? "ON" : "OFF");
}

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>
        body { font-family: Arial; padding: 20px; }
        h2 { text-align: center; }
        .section { margin-bottom: 30px; padding: 10px; border: 1px solid #ccc; border-radius: 8px; }
        .label { font-weight: bold; }
        input[type=range] { width: 100%; }
        .toggle, .slider-label { display: flex; justify-content: space-between; align-items: center; margin: 5px 0; }
        button { padding: 6px 12px; border: none; background-color: #28a745; color: white; border-radius: 4px; cursor: pointer; }
        button:hover { background-color: #218838; }
      </style></head><body>
      <h2>Dual Greenhouse Control Panel</h2>
      <div class='section'>
        <h3>Fan Speed Control</h3>
        <div class='slider-label'><span>Fan 1</span><input type='range' min='0' max='255' value='128' id='fan1Slider'></div>
        <div class='slider-label'><span>Fan 2</span><input type='range' min='0' max='255' value='128' id='fan2Slider'></div>
      </div>

      <div class='section'>
        <h3>Manual Controls</h3>
        <div class='toggle'><span>Cooler 1</span><button onclick='toggle("cooler1")'>Toggle</button></div>
        <div class='toggle'><span>Cooler 2</span><button onclick='toggle("cooler2")'>Toggle</button></div>
        <div class='toggle'><span>Humidifier 1</span><button onclick='toggle("humidifier1")'>Toggle</button></div>
        <div class='toggle'><span>Humidifier 2</span><button onclick='toggle("humidifier2")'>Toggle</button></div>
        <div class='toggle'><span>Pump 1</span><button onclick='toggle("pump1")'>Toggle</button></div>
        <div class='toggle'><span>Pump 2</span><button onclick='toggle("pump2")'>Toggle</button></div>
      </div>

      <div class='section'>
        <h3>Mode</h3>
        <button onclick='toggle("mode")'>Toggle Auto/Manual</button>
      </div>

      <script>
        document.getElementById("fan1Slider").oninput = function() {
          fetch("/setFan1Speed?value=" + this.value);
        };
        document.getElementById("fan2Slider").oninput = function() {
          fetch("/setFan2Speed?value=" + this.value);
        };
        function toggle(device) {
          fetch("/toggle?device=" + device);
        }
      </script>
      </body></html>
    )rawliteral");
  });

  server.on("/setFan1Speed", HTTP_GET, [](AsyncWebServerRequest *request){
    fan1Speed = request->getParam("value")->value().toInt();
    analogWrite(FAN1_PIN, fan1Speed);
    Serial.print("Fan 1 speed set to: "); Serial.println(fan1Speed);
    request->send(200, "text/plain", "OK");
  });

  server.on("/setFan2Speed", HTTP_GET, [](AsyncWebServerRequest *request){
    fan2Speed = request->getParam("value")->value().toInt();
    analogWrite(FAN2_PIN, fan2Speed);
    Serial.print("Fan 2 speed set to: "); Serial.println(fan2Speed);
    request->send(200, "text/plain", "OK");
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    String device = request->getParam("device")->value();
    if (device == "mode") {
      isManualMode = !isManualMode;
      Serial.print("Mode toggled to: "); Serial.println(isManualMode ? "Manual" : "Auto");
    } else if (isManualMode) {
      if (device == "cooler1") { cooler1State = !cooler1State; digitalWrite(COOLER1_PIN, cooler1State ? RELAY_ON : RELAY_OFF); }
      if (device == "cooler2") { cooler2State = !cooler2State; digitalWrite(COOLER2_PIN, cooler2State ? RELAY_ON : RELAY_OFF); }
      if (device == "humidifier1") { humidifier1State = !humidifier1State; digitalWrite(HUMIDIFIER1_PIN, humidifier1State ? RELAY_ON : RELAY_OFF); }
      if (device == "humidifier2") { humidifier2State = !humidifier2State; digitalWrite(HUMIDIFIER2_PIN, humidifier2State ? RELAY_ON : RELAY_OFF); }
      if (device == "pump1") { pump1State = !pump1State; digitalWrite(PUMP1_PIN, pump1State ? RELAY_ON : RELAY_OFF); }
      if (device == "pump2") { pump2State = !pump2State; digitalWrite(PUMP2_PIN, pump2State ? RELAY_ON : RELAY_OFF); }
      Serial.print("Toggled device: "); Serial.println(device);
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  pinMode(COOLER1_PIN, OUTPUT);
  pinMode(HUMIDIFIER1_PIN, OUTPUT);
  pinMode(PUMP1_PIN, OUTPUT);

  pinMode(COOLER2_PIN, OUTPUT);
  pinMode(HUMIDIFIER2_PIN, OUTPUT);
  pinMode(PUMP2_PIN, OUTPUT);

  pinMode(FAN1_PIN, OUTPUT);
  pinMode(FAN2_PIN, OUTPUT);

  dht1.begin();
  dht2.begin();

  WiFi.begin("iQOO Z3 5G", "123456798");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    setupWebServer();
  } else {
    Serial.println("\nWiFi not connected, running in auto mode");
  }
}

void loop() {
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  float temp2 = dht2.readTemperature();
  float hum2 = dht2.readHumidity();

  Serial.println("\nReading sensor values...");

  if (!isManualMode) {
    autoControl(temp1, hum1, temp2, hum2);
  } else {
    Serial.println("Manual mode active - skipping auto control");
  }

  delay(5000);
}
