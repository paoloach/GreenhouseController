//
// Created by paolo on 12/7/20.
//

#ifndef GREENHOUSECONTROLLER_MQTTSWITCHCONFIGURATIONMESSAGE_H
#define GREENHOUSECONTROLLER_MQTTSWITCHCONFIGURATIONMESSAGE_H


#include <cJSON.h>

class MqttSwitchConfigurationMessage {
public:
    MqttSwitchConfigurationMessage(const char * name,  const char * icon);

    cJSON * createConfig();
    char * createConfigTopicName();
    char * createCmdTopicName();
    char * stateFieldName;
private:
    const char * name;
    const char * icon;
};


#endif //GREENHOUSECONTROLLER_MQTTSWITCHCONFIGURATIONMESSAGE_H
