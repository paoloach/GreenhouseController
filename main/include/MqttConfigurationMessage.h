//
// Created by paolo on 12/7/20.
//

#ifndef GREENHOUSECONTROLLER_MQTTCONFIGURATIONMESSAGE_H
#define GREENHOUSECONTROLLER_MQTTCONFIGURATIONMESSAGE_H


#include <cJSON.h>

class MqttConfigurationMessage {
public:
    MqttConfigurationMessage(const char * name, const char * subName, const char * unitOfMeasure, const char * icon);

    cJSON * createConfig();
    char * createTopicName();
    char * stateFieldName;
private:
    const char * name;
    const char * subName;
    const char * unitOfMeas;
    const char * icon;
};


#endif //GREENHOUSECONTROLLER_MQTTCONFIGURATIONMESSAGE_H
