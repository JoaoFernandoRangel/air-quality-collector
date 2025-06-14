
#include <Arduino.h>
#include "conf.h"
#include "time.h"
#include <FirebaseClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "registerClass.h"
#include "sensorBundle.h"
using namespace std;
#include "cJSON.h"
#include "string.h"

// TODO - Implementar classe de leitura dos logs da memória flash
// TODO - Implementar classe de envio para Firebase
// TODO - Implementar subclasse de formatação de JSON para firebase e memória flash
// TODO - Validar leituras do MQ135

registerClass regClass("/registros");
sensorBundle sensor(true, false, true, false); // dht11, mq7, mq145, rtc

// User functions
void processData(AsyncResult &aResult);
String returnJsonToSave(float mq7, float mq135, float temp, float humid, uint32_t rtcUnixTime);
String getTimestamp(unsigned long &t);
void initWiFi();
// Authentication

// UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);
// FirebaseApp app;le
// WiFiClientSecure ssl_client;
// using AsyncClient = AsyncClientClass;
// AsyncClient aClient(ssl_client);
// RealtimeDatabase Database;

// Create JSON objects for storing data
object_t jsonData, obj[4];
JsonWriter writer;
QueueHandle_t dataQueue;

void rng_test_task(void *pvParameters);
void readerTask(void *pvParameters);
void senderTask(void *pvParameters);

void setup() {
    Serial.begin(115200);

    // initWiFi();
    // configTime(-10800, 0, "pool.ntp.org");
    // Create tasks for each core
    // regClass.registerInit();
    sensor.initSensors();
    sensor.calibrateCO2Reading();
    xTaskCreatePinnedToCore(rng_test_task, "Task0", 4096, nullptr, 1, nullptr, 0);
    // xTaskCreatePinnedToCore(readerTask, "Task0", 4096, nullptr, 1, nullptr, 0);
    // xTaskCreatePinnedToCore(senderTask, "Task1", 2 * 4096, nullptr, 1, nullptr,
    // 1);
}

void readerTask(void *pvParameters) {
    while (true) {
        // xQueueSend(dataQueue, &data, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(50)); // Evita busy-wait
    }
}

void rng_test_task(void *pvParameters) {
    unsigned long now[2];
    now[0] = millis();
    now[1] = millis();
    while (true) {
        if (millis() - now[0] > READ_INTERVAL) {
            sensor.pollSensors();
            Serial.println("====");
            Serial.printf("Valor de temperatura: %.2f\n", sensor.getdht11Temp());
            Serial.printf("Valor de humidade: %.2f\n", sensor.getdht11Humidade());
            Serial.printf("Leitura de CO2: %.2f\n", sensor.getMQ135Ppm());
            // Serial.println("====");
            now[0] = millis();
        }
        if (millis() - now[1] > 1.5 * READ_INTERVAL) {
            now[1] = millis();
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Evita busy-wait
    }
}

// void senderTask(void *pvParameters) {
//   // Initialize Firebase app on core 1
//   // Configure SSL client
//   ssl_client.setInsecure();
//   ssl_client.setTimeout(30);
//   ssl_client.setHandshakeTimeout(5);

//   // Initialize Firebase
//   initializeApp(aClient, app, getAuth(user_auth), processData, "🔐
//   authTask"); app.getApp<RealtimeDatabase>(Database);
//   Database.url(DATABASE_URL);
//   while (true) {
//     // Check if authentication is ready
//     app.loop();
//     if (app.ready()) {
//       if (xQueueReceive(dataQueue, &data, portMAX_DELAY)) {
//         String parentPath = "/esp_atmospheric_data/" +
//         String(data.timestamp); String tempKey = "temperature"; String humKey
//         = "humidity"; String presKey = "pressure"; String timeKey =
//         "timestamp";

//         // Create a JSON object with the data
//         writer.create(obj[0], tempKey, data.temperature);
//         writer.create(obj[1], humKey, data.humidity);
//         writer.create(obj[2], presKey, data.pressure);
//         writer.create(obj[3], timeKey, data.timestamp);
//         writer.join(jsonData, 4, obj[0], obj[1], obj[2], obj[3]);

//         Database.set<object_t>(aClient, parentPath, jsonData, processData,
//                                "RTDB_Send_Data");
//       }
//     }
//     vTaskDelay(pdMS_TO_TICKS(50)); // Verifica com frequência razoável
//   }
// }

void loop() {
    // Maintain authentication and async tasks
    vTaskSuspend(NULL);
}

void processData(AsyncResult &aResult) {
    if (!aResult.isResult()) return;

    if (aResult.isEvent())
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(),
                        aResult.eventLog().code());

    if (aResult.isDebug()) Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

    if (aResult.isError())
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(),
                        aResult.error().code());

    if (aResult.available()) Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
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

String returnJsonToSave(float mq7, float mq135, float temp, float humid, uint32_t rtcUnixTime) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "mq7_CO", mq7);
    cJSON_AddNumberToObject(root, "mq135_CO2", mq135);
    cJSON_AddNumberToObject(root, "temp_celsius", temp);
    cJSON_AddNumberToObject(root, "humid", humid);
    cJSON_AddNumberToObject(root, "rtcUnixTime", rtcUnixTime);
    String ret;
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return ret;
}
