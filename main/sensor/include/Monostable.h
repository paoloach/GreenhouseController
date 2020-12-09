//
// Created by paolo on 12/8/20.
//

#pragma once

#include <driver/gpio.h>
#include "../../sensor/include/Switch.h"
#include "../../include/MqttSwitchConfigurationMessage.h"

class SensorSetting;

class Monostable: public Switch {
private:
    static constexpr const char *TAG = "Bistable";
    gpio_num_t pinH;
    std::unique_ptr<MqttSwitchConfigurationMessage> configuration;
    bool state;
public:
    Monostable() : Switch(SensorType::MONOSTABLE), state(false) {}

    void init(SensorSetting &setting) override;

private:
    void on() override;

    void off() override;
};

