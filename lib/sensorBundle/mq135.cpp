#include "mq135.h"

mq135::mq135(uint8_t pin) {
    this->setPin(pin);
}

void mq135::setPin(uint8_t pin) {
    _pin = pin;
}

void mq135::init() {
    pinMode(_pin, INPUT);
}

void mq135::setSensitivity(float sensitivity) {
    _sensitivity = sensitivity;
}

void mq135::setRefValue(float refValue) {
    _refValue = refValue;
}

void mq135::update() {
    // int buffer = 0;
    // for (int i = 0; i < 10; i++) {
    //     buffer += analogRead(_pin);
    //     vTaskDelay(pdMS_TO_TICKS(50));
    // }
    // _sensorValue = buffer / 10;
    _sensorValue = analogRead(_pin);
    if (_sensorValue != 0) {
        _airQuality = (float)_sensorValue * _sensitivity / _resolution; // Calcular a qualidade do ar
    }else{
        _airQuality = (float)0; // Calcular a qualidade do ar
    }
}

float mq135::getAirQuality() {
    return _airQuality;
}

void mq135::calibrate() {
    int buffer;
    Serial.printf("O sensor de CO2 passará por um processo de calibração... Favor aguardar\n");
    for (int i = 0; i < 10; i++) {
        buffer += analogRead(_pin);
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    float valorSoma = (float)buffer / 10;
    // float valorSoma = 2000.0;
    _sensitivity = _refValue * (_resolution) / valorSoma;
    Serial.printf("\n Valor de calibração : %.2f\n", _sensitivity);
}
