//
// Created by paolo on 12/7/20.
//

#ifndef GREENHOUSECONTROLLER_AUTOCONFIGURATIONTOPIC_H
#define GREENHOUSECONTROLLER_AUTOCONFIGURATIONTOPIC_H

#include <esp_log.h>

class AutoconfigurationTopic {
public:
    AutoconfigurationTopic( cJSON * data, char * topicName):data(data), topicName(topicName){
        ESP_LOGI("AutoconfigurationTopic", "creating %s", topicName);
    }
    ~AutoconfigurationTopic(){
        ESP_LOGI("AutoconfigurationTopic", "deleting %s", topicName);
        if (data != nullptr){
            cJSON_Delete(data);
        }
        if (topicName != nullptr){
            free(topicName);
        }
    }

    cJSON * data;
    char * topicName;
};


#endif //GREENHOUSECONTROLLER_AUTOCONFIGURATIONTOPIC_H
