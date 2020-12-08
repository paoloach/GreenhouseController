//
// Created by paolo on 12/8/20.
//

#pragma once

#include "../../include/Sensor.h"
#include "../../include/MqttSwitchConfigurationMessage.h"

class Switch  : public Sensor{
private:
    static constexpr const char * TAG ="Switch";
protected:
    std::unique_ptr<MqttSwitchConfigurationMessage> configuration;
    bool state;
public:
    Switch(SensorType type):Sensor(type), state(false){}
    void step() override;
    std::vector<std::unique_ptr<AutoconfigurationTopic>> autoconfigure() override;
    void setState(cJSON *pJson) override;
    std::vector<std::unique_ptr<SubscribingTopic>> subscribingTopics() override;
    void cmdHandler(char * data, int len);

protected:
    virtual void on()=0;
    virtual void off()=0;
};

