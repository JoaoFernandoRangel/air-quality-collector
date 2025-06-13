#include "Arduino.h"
#include "conf.h"

class mq135 {
   public:
    mq135(uint8_t pin);
    void init();
    void setPin(uint8_t pin);
    void setSensitivity(int sensitivity);
    void update();
    int getAirQuality();

   private:
    uint8_t _pin;
    int _movingAvarageArray[10];
    int _sensorValue;
    int _sensitivity = 200; // Varia com a calibração
    int _airQuality;
    int _resolution = 4096; // 12-bit ADC resolution
};
