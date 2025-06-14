#include "Arduino.h"
#include "conf.h"

class mq135 {
   public:
    mq135(uint8_t pin);
    void init();
    void setPin(uint8_t pin);
    void setSensitivity(float sensitivity);
    void setRefValue(float sensitivity);
    void update();
    float getAirQuality();
    void calibrate();

   private:
    uint8_t _pin;
    int _movingAvarageArray[10];
    int _sensorValue;
    float _sensitivity = 200.0; // Varia com a calibração
    float _airQuality;
    int _resolution = 4096; // 12-bit ADC resolution
    int _refValue = 440;
};
