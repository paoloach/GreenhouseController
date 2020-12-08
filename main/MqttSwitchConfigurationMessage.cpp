//
// Created by paolo on 12/7/20.
//

#include <cstdio>
#include <cstring>
#include "include/MqttSwitchConfigurationMessage.h"
#include "include/Settings.h"

MqttSwitchConfigurationMessage::MqttSwitchConfigurationMessage(const char *name, const char *icon) :
    name(name), icon(icon){

        char buffer[100];
        sprintf(buffer, "%s", name);
        stateFieldName = strdup(buffer);

}

char *MqttSwitchConfigurationMessage::createConfigTopicName() {
    char buffer[200];
    sprintf(buffer, "homeassistant/switch/%s-%s-%s/config", Settings::settings.name, Settings::settings.strMac, name);
    return strdup(buffer);
}

char *MqttSwitchConfigurationMessage::createCmdTopicName() {
    char buffer[200];
    sprintf(buffer, "homeassistant/switch/%s-%s-%s/set", Settings::settings.name, Settings::settings.strMac, name);
    return strdup(buffer);
}

cJSON *MqttSwitchConfigurationMessage::createConfig() {
    cJSON * json = cJSON_CreateObject();
    char buffer[200];

    sprintf(buffer, "%s-%s", Settings::settings.name, stateFieldName);
    cJSON_AddStringToObject(json, "name", buffer);
    sprintf(buffer, "%s-%s-%s", Settings::settings.name, Settings::settings.strMac, stateFieldName);
    cJSON_AddStringToObject(json, "uniq_id", buffer);
    cJSON_AddItemToObject(json, "dev", Settings::settings.devDescription());
    cJSON_AddStringToObject(json, "stat_t", Settings::settings.mqttStateTopic);
    char * cmdTopic = createCmdTopicName();
    cJSON_AddStringToObject(json, "command_topic", cmdTopic);
    free(cmdTopic);
    if (icon != nullptr) {
        cJSON_AddStringToObject(json, "ic", icon);
    }
    cJSON_AddStringToObject(json, "pl_off", "OFF");
    cJSON_AddStringToObject(json, "pl_on", "ON");
    sprintf(buffer, "{{value_json.%s}}", stateFieldName);
    cJSON_AddStringToObject(json, "val_tpl", buffer);

    return json;
}
