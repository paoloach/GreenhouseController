//
// Created by paolo on 12/6/20.
//

#include <esp_event.h>
#include <mqtt_client.h>
#include <esp_log.h>
#include "include/Mqtt.h"
#include "include/Settings.h"
#include "include/GroupSignals.h"

Mqtt * mqttClient;

void Mqtt::init() {
    esp_mqtt_client_config_t mqttCfg;
    memset(&mqttCfg, 0, sizeof (mqttCfg));
    mqttCfg.uri = Settings::settings.mqttBrokerUrl;
    mqttCfg.username = Settings::settings.mqttUsername;
    mqttCfg.password = Settings::settings.mqttPassword;
    client = esp_mqtt_client_init(&mqttCfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, eventHandler, this);


    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE,portMAX_DELAY );

    esp_mqtt_client_start(client);
}

void Mqtt::eventHandler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
    Mqtt * This = static_cast<Mqtt*>(event_handler_arg);
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            This->eventConnect(event);
            break;
        case MQTT_EVENT_DISCONNECTED:
            This->eventDisconnect();
            break;
        case MQTT_EVENT_SUBSCRIBED:
            This->eventSubscribed(event);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            This->eventUnsubscribed();
            break;
        case MQTT_EVENT_PUBLISHED:
            This->eventPublished();
            break;
        case MQTT_EVENT_DATA:
            This->eventData(event);
            break;
        case MQTT_EVENT_ERROR:
            This->eventError();
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void Mqtt::eventDisconnect() {
    ESP_LOGE(TAG,"Disconnected");

}

void Mqtt::eventSubscribed(esp_mqtt_event_handle_t event) {
    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
//    msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
//    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

}

void Mqtt::eventUnsubscribed() {

}

void Mqtt::eventData(esp_mqtt_event_handle_t event) {
    ESP_LOGI(TAG, "NEW DATA");
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);
}

void Mqtt::eventError() {
    ESP_LOGE(TAG, "ERROR");
}

void Mqtt::eventConnect(esp_mqtt_event_handle_t event) {
    ESP_LOGI(TAG, "CONNECTED");


    for(uint8_t i=0; i<totSensors; i++){
        auto autoconfigures = sensors[i]->autoconfigure();
        for(auto & json: autoconfigures){
            auto data = cJSON_Print(json->data);
            esp_mqtt_client_publish(client, json->topicName, data, 0, 1, 0);
        }
    }

//    msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
//    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
//
//    msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
//    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
//
//    msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
//    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
//
//    msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
//    ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
}

void Mqtt::eventPublished() {

}


void initMqtt(void) {
    if (Settings::settings.mqttEnable){
        mqttClient = new Mqtt();
        mqttClient->init();
    }

}
