#include "sensorBundle.h"

sensorBundle::sensorBundle() {}

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