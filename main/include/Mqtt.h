//
// Created by paolo on 12/6/20.
//

#ifndef GREENHOUSECONTROLLER_MQTT_H
#define GREENHOUSECONTROLLER_MQTT_H


#include <mqtt_client.h>
#include <cJSON.h>
#include "Sensor.h"

#ifdef __cplusplus

class Mqtt {
public:
    void init();

private:
    static constexpr const char * TAG="MQTT";
    esp_mqtt_client_handle_t client;
    std::vector<std::unique_ptr<SubscribingTopic>> subscribingTopic;

    static void  eventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    void eventConnect(esp_mqtt_event_handle_t ptr);
    void eventDisconnect();
    void eventSubscribed(esp_mqtt_event_handle_t ptr);
    void eventUnsubscribed();
    void eventPublished();
    void eventData(esp_mqtt_event_handle_t ptr);
    void eventError();
    static void sendStatus(void *);
};

extern Mqtt * mqttClient;

extern "C" void initMqtt();

#else

void initMqtt(void);

#endif

#endif //GREENHOUSECONTROLLER_MQTT_H
