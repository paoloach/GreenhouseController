//
// Created by paolo on 12/5/20.
//

#ifndef GREENHOUSECONTROLLER_SENSORSETTING_H
#define GREENHOUSECONTROLLER_SENSORSETTING_H

#include <string_view>
#include <memory>
#include <driver/gpio.h>
#include "Sensor.h"


enum class SensorType {
    DHT11,
    LIGHT,
    HEATER
};

class SensorSetting {
public:
    std::string_view name;
    std::unique_ptr<gpio_num_t[]> pins;
    SensorType sensorType;
};



#endif //GREENHOUSECONTROLLER_SENSORSETTING_H
