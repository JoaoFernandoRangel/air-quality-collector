#include "sensorBundle.h"

sensorBundle::sensorBundle(bool _DHT, bool MQ7, bool MQ135, bool RTC) {
    _dht11_enabled = _DHT;
    _mq7_enabled = MQ7;
    _mq135_enabled = MQ135;
    _rtc_enabled = RTC;
    if (_dht11_enabled) {
        _dht11 = new DHT(DHT_PIN, DHT11);
    }
}

void sensorBundle::initSensors() {
    _dht11->begin();
}

void sensorBundle::pollSensors() {
    if (_dht11_enabled) {
        _dht11_temp = _dht11->readTemperature();
        _dht11_hum = _dht11->readHumidity();
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