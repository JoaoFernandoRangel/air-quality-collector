#include "sensorBundle.h"

sensorBundle::sensorBundle(bool DH, bool mq7, bool _MQ135, bool rtc) {
    _dht11_enabled = DH;
    _mq7_enabled = mq7;
    _mq135_enabled = _MQ135;
    _rtc_enabled = rtc;
    if (_dht11_enabled) {
        _dht11 = new DHT(DHT_PIN, DHT11);
    }
    if (_mq7_enabled) {
        _mq7 = new MQUnifiedsensor("ESP32", 3.3, 12, MQ7_PIN, "MQ-7"); // 3.3V, 12-bit ADC, MQ-7 sensor
    }
    if (_mq135_enabled) {
        _mq135 = new mq135(MQ135_PIN);
    }
    if (_rtc_enabled) {
        _rtc = new RTC_DS1307();
    }
}

void sensorBundle::calibrateCO2Reading(){
    _mq135->calibrate();
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
        for (int i = 0; i < 10; i++) {
            _mq7->update();
            _calcR0 += _mq7->calibrate(27.5); // Calibrating with clean air
        }
        _mq7->setR0(_calcR0 / 10); // Set the average R0 value
    }
    if (_mq135_enabled) {
        _mq135->init();
    }
    if (_rtc_enabled) {
        _rtc->begin();
    }
}

void sensorBundle::pollSensors() {
    if (_dht11_enabled) {
        _dht11_temp = _dht11->readTemperature();
        _dht11_hum = _dht11->readHumidity();
    }
    if (_mq7_enabled) {
        _mq7->update();
        _mq7_ppm = _mq7->readSensor(); //Le valor de CO
    }
    if (_mq135_enabled) {
        _mq135->update();
        _mq135_ppm = _mq135->getAirQuality(); // Le valor de CO2
    }
    if (_rtc_enabled) {
        this->setDateTime(_rtc->now()); //Busca no relógio o UTC
        _timestampUnixTime = _timeNow.unixtime();
    }
}

void sensorBundle::setDateTime(DateTime d) {
    _timeNow = d;
}

DateTime sensorBundle::getDateTime() {
    return _timeNow;
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
uint32_t sensorBundle::getRTCTimestamp() {
    return _timestampUnixTime;
}
