//
// Created by paolo on 12/8/20.
//

#include "include/Monostable.h"
#include <memory>
#include "../include/Settings.h"

using namespace std;

void Monostable::init(SensorSetting &setting) {
    name = setting.name;
    pinH = setting.pins[0];
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = BIT(pinH)  ;
    gpioConfig.mode = GPIO_MODE_OUTPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpioConfig);

    off();
    configuration = make_unique<MqttSwitchConfigurationMessage>(name, "mdi:lightbulb-outline");
}


void Monostable::on() {
    gpio_set_level(pinH, 1);
    state= true;
}

void Monostable::off() {
    gpio_set_level(pinH, 0);
    state= false;
}