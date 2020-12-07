//
// Created by paolo on 12/7/20.
//

#include <cstring>
#include "include/MqttConfigurationMessage.h"
#include "include/Settings.h"

cJSON *MqttConfigurationMessage::createConfig() {
    cJSON * tempJson = cJSON_CreateObject();
    char buffer[200];

    sprintf(buffer, "%s-%s", Settings::settings.name, stateFieldName);
    cJSON_AddStringToObject(tempJson, "name", buffer);
    sprintf(buffer, "%s-%s-%s", Settings::settings.name, Settings::settings.strMac, stateFieldName);
    cJSON_AddStringToObject(tempJson, "uniq_id", buffer);
    cJSON * devJson = cJSON_CreateObject();
    sprintf(buffer, "%s-%s", Settings::settings.name, Settings::settings.strMac);
    cJSON_AddStringToObject(devJson, "ids",buffer);
    cJSON_AddItemToObject(tempJson, "dev",devJson);

    cJSON_AddStringToObject(tempJson, "stat_t",  Settings::settings.mqttStateTopic);
    cJSON_AddStringToObject(tempJson, "unit_of_meas", unitOfMeas);
    if (icon != nullptr) {
        cJSON_AddStringToObject(tempJson, "ic", icon);
    }
    cJSON_AddStringToObject(tempJson, "frc_upd", "false");
    sprintf(buffer, "{{value_json.%s}}", stateFieldName);
    cJSON_AddStringToObject(tempJson, "val_tpl", buffer);

    return tempJson;
}

char *MqttConfigurationMessage::createTopicName() {
    char buffer[200];
    if (subName != nullptr) {
        sprintf(buffer, "homeassistant/sensor/%s-%s-%s-%s/config", Settings::settings.name, Settings::settings.strMac,
                name, subName);
    } else {
        sprintf(buffer, "homeassistant/sensor/%s-%s-%s/config", Settings::settings.name, Settings::settings.strMac,
                name);
    }
    return strdup(buffer);
}

MqttConfigurationMessage::MqttConfigurationMessage(const char *name, const char *subName, const char *unitOfMeasure,
                                                   const char *icon) :
        name(name), subName(subName), unitOfMeas(unitOfMeasure), icon(icon){

    char buffer[100];
    if (subName != nullptr) {
        sprintf(buffer, "%s-%s", name, subName);
    } else {
        sprintf(buffer, "%s", name);
    }
    stateFieldName = strdup(buffer);
}

