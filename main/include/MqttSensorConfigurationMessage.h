//
// Created by paolo on 12/7/20.
//

#ifndef GREENHOUSECONTROLLER_MQTTSENSORCONFIGURATIONMESSAGE_H
#define GREENHOUSECONTROLLER_MQTTSENSORCONFIGURATIONMESSAGE_H


#include <cJSON.h>

class MqttSensorConfigurationMessage {
public:
    MqttSensorConfigurationMessage(const char * name, const char * subName, const char * unitOfMeasure, const char * icon);

    cJSON * createConfig();
    char * createConfigTopicName();
    char * stateFieldName;
private:
    const char * name;
    const char * subName;
    const char * unitOfMeas;
    const char * icon;
};


#endif //GREENHOUSECONTROLLER_MQTTSENSORCONFIGURATIONMESSAGE_H
