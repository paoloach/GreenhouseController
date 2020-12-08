//
// Created by paolo on 12/7/20.
//

#include <memory>
#include <cstring>
#include "include/Bistable.h"
#include "../include/MqttSwitchConfigurationMessage.h"
#include "../include/Settings.h"

using namespace std;

void Bistable::init(SensorSetting &setting) {
    name = setting.name;
    pinH = setting.pins[0];
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = BIT(pinH)  ;
    gpioConfig.mode = GPIO_MODE_OUTPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpioConfig);

    pinL = setting.pins[1];
    gpioConfig.pin_bit_mask = BIT(pinL)  ;
    gpioConfig.mode = GPIO_MODE_OUTPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpioConfig);
    off();
    configuration = make_unique<MqttSwitchConfigurationMessage>(name, "mdi:lightbulb-outline");
}


void Bistable::on() {
    gpio_set_level(pinH, 1);
    gpio_set_level(pinL, 0);
    state= true;
}

void Bistable::off() {
    gpio_set_level(pinH, 0);
    gpio_set_level(pinL, 1);
    state= false;
}

