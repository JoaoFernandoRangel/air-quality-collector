#include "sensorBundle.h"

sensorBundle::sensorBundle(bool _DHT, bool MQ7, bool MQ135, bool RTC) {
    _dht11_enabled = _DHT;
    _mq7_enabled = MQ7;
    _mq135_enabled = MQ135;
    _rtc_enabled = RTC;
    if (_dht11_enabled) {
        _dht11 = new DHT(DHT_PIN, DHT11);
    }
    if (_mq7_enabled) {
        _mq7 = new MQUnifiedsensor("ESP32", 3.3, 12, MQ7_PIN, "MQ-7"); // 3.3V, 12-bit ADC, MQ-7 sensor
    }
}

void sensorBundle::initSensors() {
    if (_dht11_enabled) {
        _dht11->begin();
    }
    if (_mq7_enabled) {
        _mq7->setRegressionMethod(1); // PPM = a * ratio^b
        _mq7->setA(99.042);
        _mq7->setB(-1.518);
        _mq7->init();
        int _calcR0 = 0;
        for(int i = 0; i < 10; i++) {
            _mq7->update();
            _calcR0 += _mq7->calibrate(27.5); // Calibrating with clean air
        }
        _mq7->setR0(_calcR0 / 10); // Set the average R0 value
    }
}

void sensorBundle::pollSensors() {
    if (_dht11_enabled) {
        _dht11_temp = _dht11->readTemperature();
        _dht11_hum = _dht11->readHumidity();
    }
    if(_mq7_enabled){
        _mq7->update();
        _mq7_ppm = _mq7->readSensor();
    }
}

float sensorBundle::getMQ7Ppm() {
    return _mq7_ppm;
}
float sensorBundle::getMQ135Ppm() {
    return _mq135_ppm;
}
float sensorBundle::getdht11Temp() {
    return _dht11_temp;
}
float sensorBundle::getdht11Humidade() {
    return _dht11_hum;
}
unsigned long sensorBundle::getRTCTimestamp() {
    return _timestamp;
}