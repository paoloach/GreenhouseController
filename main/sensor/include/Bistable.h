//
// Created by paolo on 12/7/20.
//

#ifndef GREENHOUSECONTROLLER_BISTABLE_H
#define GREENHOUSECONTROLLER_BISTABLE_H


#include <driver/gpio.h>
#include "../../include/Sensor.h"
#include "../../include/MqttSwitchConfigurationMessage.h"

class SensorSetting;

class Bistable : public Sensor{
private:
    static constexpr const char * TAG ="Bistable";
    gpio_num_t pinH;
    gpio_num_t pinL;
    std::unique_ptr<MqttSwitchConfigurationMessage> configuration;
    bool state;
public:
    Bistable():Sensor(SensorType::BISTABLE), state(false){}
    void init(SensorSetting & setting) override;
    void step() override;
    std::vector<std::unique_ptr<AutoconfigurationTopic>> autoconfigure() override;
    void setState(cJSON *pJson) override;
    std::vector<std::unique_ptr<SubscribingTopic>> subscribingTopics() override;
    void cmdHandler(char * data, int len);

private:
    void on();
    void off();
};


#endif //GREENHOUSECONTROLLER_BISTABLE_H
