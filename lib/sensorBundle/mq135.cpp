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

void mq135::setSensitivity(int sensitivity) {
    _sensitivity = sensitivity;
}

void mq135::update() {
    _sensorValue = analogRead(_pin);
    _airQuality = _sensorValue * _sensitivity / _resolution; // Calcular a qualidade do ar
}

int mq135::getAirQuality() {
    return _airQuality;
}