#include "Arduino.h"
#include "conf.h"
#include "Wire.h"
#include "DHT.h"
// Lista de sensores
//  MQ-7: Monóxido de Carbono - ppm
//  MQ-135: Gás Amônia, Óxido Nítrico, Álcool, Benzeno, Dióxido de Carbono e Fumaça - ppm
//  DHT11: Temperatura, humidade
//  RTC- : timestamp (unix)

#define DHT_PIN 33 // Pino do DHT11
class sensorBundle {
   public:
    sensorBundle(bool DH, bool MQ7, bool MQ135, bool RTC);
    void initSensors();
    void pollSensors();
    float getMQ7Ppm();
    float getMQ135Ppm();
    float getdht11Temp();
    float getdht11Humidade();
    unsigned long getRTCTimestamp();

   private:
    DHT *_dht11;
    float _mq7_ppm, _mq135_ppm, _dht11_temp, _dht11_hum;
    unsigned long _timestamp;
    bool _dht11_enabled, _mq7_enabled, _mq135_enabled, _rtc_enabled;
};