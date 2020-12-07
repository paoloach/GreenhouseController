//
// Created by paolo on 12/2/20.
//

#ifndef GREENHOUSECONTROLLER_DHT11_H
#define GREENHOUSECONTROLLER_DHT11_H

#include <driver/gpio.h>

#ifdef __cplusplus

#include <list>
#include "Sensor.h"


class SensorSetting;

enum class Dht11Result {
    VALID,
    INVALID_TIMEOUT,
    PARITY_ERROR
};

class Dht11 : public Sensor{
private:
    static constexpr const char * TEMP_SENSOR_NAME="temperature";
    static constexpr const char * HUMIDITY_SENSOR_NAME="humidity";
    gpio_num_t pin;
public:
    Dht11():Sensor(SensorType::DHT11){}
    void init(SensorSetting & setting) override;
    void step() override;
    std::vector<std::unique_ptr<AutoconfigurationTopic>> autoconfigure() override;
    Dht11Result  read();

    float humidity;
    float temp;

private:
    void start();
    Dht11Result waitFor1(uint8_t timeout);
    Dht11Result waitFor0(uint8_t timeout);
    static Dht11Result checkParity(const uint8_t *data);
    cJSON * createTempConfiguration();
    cJSON * createHumidityConfiguration();
    char * createTempTopicConfiguration();
    char * createHumidityTopicConfiguration();
};
#endif


#endif //GREENHOUSECONTROLLER_DHT11_H
