//
// Created by paolo on 12/8/20.
//

#ifndef GREENHOUSECONTROLLER_SUBSCRIBINGTOPIC_H
#define GREENHOUSECONTROLLER_SUBSCRIBINGTOPIC_H

#include <functional>
#include <esp_log.h>

class SubscribingTopic {
public:
    char * topicName;
    std::function<void( uint8_t * )> handler;

    SubscribingTopic():topicName(nullptr), handler(nullptr){
        ESP_LOGI("SubscribingTopic", "default creating %s", topicName);
    }

    SubscribingTopic(char * topicName, std::function<void( uint8_t * )> && handler):
     topicName(topicName), handler(std::move(handler)){
        ESP_LOGI("SubscribingTopic", "creating %s", topicName);
    }

    ~SubscribingTopic(){
        ESP_LOGI("SubscribingTopic", "deleting %s", topicName);
        if (topicName != nullptr){
            free(topicName);
        }
    }
};

#endif //GREENHOUSECONTROLLER_SUBSCRIBINGTOPIC_H
