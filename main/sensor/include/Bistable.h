//
// Created by paolo on 12/7/20.
//

#pragma once

#include <driver/gpio.h>
#include "../../sensor/include/Switch.h"
#include "../../include/MqttSwitchConfigurationMessage.h"

class SensorSetting;

class Bistable : public Switch{
private:
    static constexpr const char * TAG ="Bistable";
    gpio_num_t pinH;
    gpio_num_t pinL;
    std::unique_ptr<MqttSwitchConfigurationMessage> configuration;
    bool state;
public:
    Bistable():Switch(SensorType::BISTABLE), state(false){}
    void init(SensorSetting & setting) override;

private:
    void on() override;
    void off() override;
};


