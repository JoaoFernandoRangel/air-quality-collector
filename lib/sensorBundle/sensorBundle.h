#include "Arduino.h"
#include "conf.h"
#include "DHT.h"
#include "MQUnifiedsensor.h"
#include "mq135.h"
#include "Wire.h"
#include "RTClib.h"
// Lista de sensores
//  MQ-7: Monóxido de Carbono - ppm
//  MQ-135: Gás Amônia, Óxido Nítrico, Álcool, Benzeno, Dióxido de Carbono e Fumaça - ppm
//  DHT11: Temperatura, humidade
//  RTC- : timestamp (unix)

#define DHT_PIN 33   // Pino do DHT11
#define MQ7_PIN 33   // Pino do DHT11
#define MQ135_PIN 35 // Pino do DHT11

class sensorBundle {
   public:
    sensorBundle(bool DH = false, bool mq7 = false, bool _MQ135 = false, bool rtc = false);
    void initSensors();
    void pollSensors();
    float getMQ7Ppm();
    float getMQ135Ppm();
    float getdht11Temp();
    float getdht11Humidade();
    uint32_t getRTCTimestamp();
    void setDateTime(DateTime d);
    DateTime getDateTime();
    void calibrateCO2Reading();

   private:
    DHT *_dht11;
    MQUnifiedsensor *_mq7;
    mq135 *_mq135;
    RTC_DS1307 *_rtc;
    DateTime _timeNow;
    float _mq7_ppm, _mq135_ppm, _dht11_temp, _dht11_hum;
    uint32_t _timestampUnixTime;
    bool _dht11_enabled, _mq7_enabled, _mq135_enabled, _rtc_enabled;
};
