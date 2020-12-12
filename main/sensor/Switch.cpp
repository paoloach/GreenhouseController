//
// Created by paolo on 12/8/20.
//

#include <cstring>
#include "include/Switch.h"


using namespace std;

void Switch::step() {

}

std::vector<std::unique_ptr<AutoconfigurationTopic>> Switch::autoconfigure() {
    auto jsons = vector<std::unique_ptr<AutoconfigurationTopic>>();
    ESP_LOGI(TAG,"Creating configuration for %s", configuration->createConfigTopicName());
    jsons.push_back(make_unique<AutoconfigurationTopic>(configuration->createConfig(), configuration->createConfigTopicName()));

    return jsons;
}

void Switch::setState(cJSON *pJson) {
    if (state)
        cJSON_AddStringToObject(pJson, configuration->stateFieldName, "ON");
    else
        cJSON_AddStringToObject(pJson, configuration->stateFieldName, "OFF");
}


std::vector<unique_ptr<SubscribingTopic>> Switch::subscribingTopics() {

    auto subscribingTopics = std::vector<std::unique_ptr<SubscribingTopic>>();
    subscribingTopics.push_back( make_unique<SubscribingTopic>(configuration->createCmdTopicName(), [this](char *data, int len){ cmdHandler(data,len);}));

    return subscribingTopics;
}

void Switch::cmdHandler(char *data, int len) {
    if (memcmp(data, "ON", 2)==0){
        on();
    } else if (memcmp(data, "OFF", 3)==0){
        off();
    } else {
        char error[len+1];
        memcpy(error, data, len);
        error[len]=0;
        ESP_LOGE(TAG, "Invalid cmd: %s", error);
    }
}
