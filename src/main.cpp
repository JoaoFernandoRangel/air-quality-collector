
#include "conf.h"
#include "time.h"
#include <Arduino.h>
#include <FirebaseClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>

// User functions
void processData(AsyncResult &aResult);
String getTimestamp(unsigned long &t);
void initWiFi();
// Authentication
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;

// Timer variables for sending data every 10 seconds
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // 10 seconds in milliseconds

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String tempPath = "/temperature";
String humPath = "/humidity";
String presPath = "/pressure";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

String timestamp;

const char *ntpServer = "pool.ntp.org";

float temperature;
float humidity;
float pressure;

// Create JSON objects for storing data
object_t jsonData, obj1, obj2, obj3, obj4;
JsonWriter writer;
void rng_test_task(void *pvParameters);
void readerTask(void *pvParameters);
void senderTask(void *pvParameters);
QueueHandle_t dataQueue;

struct SensorData {
  float temperature;
  float humidity;
  float pressure;
  unsigned long timestamp;
};

void setup() {
  Serial.begin(115200);

  initWiFi();
  configTime(-10800, 0, ntpServer);

  // Configure SSL client
  ssl_client.setInsecure();
  ssl_client.setTimeout(30);
  ssl_client.setHandshakeTimeout(5);

  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  // Create tasks for each core
  // xTaskCreatePinnedToCore(rng_test_task, "Task0", 4096, nullptr, 1, nullptr, 0);
  xTaskCreatePinnedToCore(readerTask, "Task0", 4096, nullptr, 1, nullptr, 0);
  xTaskCreatePinnedToCore(senderTask, "Task1", 2 * 4096, nullptr, 1, nullptr,
                          1);
  dataQueue =
      xQueueCreate(2, sizeof(SensorData)); // Create a queue for sensor data
}

SensorData data;

void readerTask(void *pvParameters) {

  while (true) {

    // xQueueSend(dataQueue, &data, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(50)); // Evita busy-wait
  }
}

void rng_test_task(void *pvParameters) {
  unsigned long now = millis();

  while (true) {
    if (millis() - now > 1000) {
      now = millis();

      for (int i = 0; i < 4; i++) {
        // Gera dados normalizados (0.0 a 1.0)
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        if (i == 0)
          data.temperature = 20.0 + r * 10.0; // 20.0 a 30.0
        if (i == 1)
          data.humidity = 40.0 + r * 20.0; // 40.0 a 60.0
        if (i == 2)
          data.pressure = 950.0 + r * 100.0; // 950 a 1050
      }

      getTimestamp(data.timestamp); // Atualiza timestamp

      // Envia struct para a fila
      xQueueSend(dataQueue, &data, portMAX_DELAY);
    }

    vTaskDelay(pdMS_TO_TICKS(50)); // Evita busy-wait
  }
}

void senderTask(void *pvParameters) {
  // Initialize Firebase app on core 1
  while (true) {
    app.loop();
    // Check if authentication is ready
    if (app.ready()) {
      if (xQueueReceive(dataQueue, &data, portMAX_DELAY)) {
        lastSendTime = millis();

        String parentPath = "/temp_atmospheric_data/" + String(data.timestamp);
        String tempPath = "temperature";
        String humPath = "humidity";
        String presPath = "pressure";
        String timePath = "timestamp";

        // Create a JSON object with the data
        writer.create(obj1, tempPath, data.temperature);
        writer.create(obj2, humPath, data.humidity);
        writer.create(obj3, presPath, data.pressure);
        writer.create(obj4, timePath, data.timestamp);
        writer.join(jsonData, 4, obj1, obj2, obj3, obj4);

        Database.set<object_t>(aClient, parentPath, jsonData, processData,
                               "RTDB_Send_Data");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Verifica com frequência razoável
  }
}

void loop() {
  // Maintain authentication and async tasks
}

void processData(AsyncResult &aResult) {
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n",
                    aResult.uid().c_str(), aResult.eventLog().message().c_str(),
                    aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(),
                    aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n",
                    aResult.uid().c_str(), aResult.error().message().c_str(),
                    aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(),
                    aResult.c_str());
}

// Initialize WiFi

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
}
// Função que retorna o timestamp como String no formato "YYYY-MM-DD HH:MM:SS"
String getTimestamp(unsigned long &t) {
  struct tm timeinfo;
  time_t now;

  if (!getLocalTime(&timeinfo)) {
    t = 0;
    return "0000-00-00 00:00:00";
  }

  time(&now);
  t = static_cast<unsigned long>(now);

  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

  return String(timestamp);
}
