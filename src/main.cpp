
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <Wire.h>
#include "time.h"
#include "conf.h"

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

const char* ntpServer = "pool.ntp.org";

float temperature;
float humidity;
float pressure;

// Create JSON objects for storing data
object_t jsonData, obj1, obj2, obj3, obj4;
JsonWriter writer;

void setup(){
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
}

void loop(){
  // Maintain authentication and async tasks
  app.loop();

  // Check if authentication is ready
  if (app.ready()){

    // Periodic data sending every 10 seconds
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      // Update the last send time
      lastSendTime = currentTime;

      uid = app.getUid().c_str();

      // Update database path
      databasePath = "/temp_atmospheric_data/";
      unsigned long tag;
      //Get current timestamp
      timestamp = getTimestamp(tag);

      parentPath= databasePath + "/" + String(tag);

      // Get sensor readings
      temperature = millis() / 1000;
      humidity = millis() / 10000;
      pressure = millis() / 15000;

      // Create a JSON object with the data
      writer.create(obj1, tempPath, temperature);
      writer.create(obj2, humPath, humidity);
      writer.create(obj3, presPath, pressure);
      writer.create(obj4, timePath, timestamp);
      writer.join(jsonData, 4, obj1, obj2, obj3, obj4);

      Database.set<object_t>(aClient, parentPath, jsonData, processData, "RTDB_Send_Data");
    }
  }
}

void processData(AsyncResult &aResult){
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
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
