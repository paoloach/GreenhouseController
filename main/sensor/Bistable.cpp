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

    configuration = make_unique<MqttSwitchConfigurationMessage>(name, "mdi:lightbulb-outline");
}

void Bistable::step() {

}

std::vector<std::unique_ptr<AutoconfigurationTopic>> Bistable::autoconfigure() {
    auto jsons = vector<std::unique_ptr<AutoconfigurationTopic>>();

    jsons.push_back(make_unique<AutoconfigurationTopic>(configuration->createConfig(), configuration->createConfigTopicName()));

    return jsons;
}

void Bistable::setState(cJSON *pJson) {
    if (state)
        cJSON_AddStringToObject(pJson, configuration->stateFieldName, "ON");
    else
        cJSON_AddStringToObject(pJson, configuration->stateFieldName, "OFF");
}

void Bistable::on() {
    gpio_set_level(pinH, 1);
    gpio_set_level(pinL, 0);
}

void Bistable::off() {
    gpio_set_level(pinH, 0);
    gpio_set_level(pinL, 1);
}

std::vector<unique_ptr<SubscribingTopic>> Bistable::subscribingTopics() {

    auto subscribingTopics = std::vector<std::unique_ptr<SubscribingTopic>>();
    subscribingTopics.push_back( make_unique<SubscribingTopic>(configuration->createCmdTopicName(), [this](char *data, int len){ cmdHandler(data,len);}));

    return subscribingTopics;
}

void Bistable::cmdHandler(char *data, int len) {
    if (memcmp(data, "ON", 2)==0){
        state= true;
    } else if (memcmp(data, "OFF", 3)==0){
        state=false;
    } else {
        char error[len+1];
        memcpy(error, data, len);
        error[len]=0;
        ESP_LOGE(TAG, "Invalid cmd: %s", error);
    }
}
