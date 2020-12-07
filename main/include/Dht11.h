//
// Created by paolo on 12/2/20.
//

#ifndef GREENHOUSECONTROLLER_DHT11_H
#define GREENHOUSECONTROLLER_DHT11_H

#include <driver/gpio.h>

#ifdef __cplusplus

#include "Sensor.h"

class SensorSetting;

enum class Dht11Result {
    VALID,
    INVALID_TIMEOUT,
    PARITY_ERROR
};

class Dht11 : public Sensor{
private:
    gpio_num_t pin;
public:
    void init(SensorSetting & setting) override;
    void step() override;
    Dht11Result  read();
    float humidity;
    float temp;

private:
    void start();
    Dht11Result waitFor1(uint8_t timeout);
    Dht11Result waitFor0(uint8_t timeout);
    static Dht11Result checkParity(const uint8_t *data);
};
#endif


#endif //GREENHOUSECONTROLLER_DHT11_H
