//
// Created by paolo on 12/2/20.
//

#include <esp32/rom/ets_sys.h>
#include <esp_timer.h>
#include <soc_log.h>
#include <cstring>
#include "include/Dht11.h"

#include "../include/SensorSetting.h"
#include "../include/Settings.h"

static const char *TAG = "DHT11";

using namespace std;

Dht11Result Dht11::read() {
    start();
    if (waitFor0(80) == Dht11Result::INVALID_TIMEOUT){
        ESP_LOGE(TAG,"%s:%d timeout", __FILE__,__LINE__);
        return Dht11Result::INVALID_TIMEOUT;
    }
    if (waitFor1(100) == Dht11Result::INVALID_TIMEOUT){
        ESP_LOGE(TAG,"%s:%d timeout", __FILE__,__LINE__);
        return Dht11Result::INVALID_TIMEOUT;
    }
    if (waitFor0(80) == Dht11Result::INVALID_TIMEOUT){
        ESP_LOGE(TAG,"%s:%d timeout", __FILE__,__LINE__);
        return Dht11Result::INVALID_TIMEOUT;
    }
    /* Read response */
    uint8_t data[5];
    uint8_t dataIndex=0;
    uint8_t bitIndex=0;
    for(int i = 0; i < 40; i++) {
        if (waitFor1(80) == Dht11Result::INVALID_TIMEOUT){
            ESP_LOGE(TAG, "timeout error at bit %d", i);
            ESP_LOGE(TAG,"%s:%d timeout", __FILE__,__LINE__);
            return Dht11Result::INVALID_TIMEOUT;
        }
        int64_t start = esp_timer_get_time();
        if (waitFor0(100) == Dht11Result::INVALID_TIMEOUT){
            ESP_LOGE(TAG, "timeout error at bit %d", i);
            ESP_LOGE(TAG,"%s:%d timeout", __FILE__,__LINE__);
            return Dht11Result::INVALID_TIMEOUT;
        }
        int64_t end = esp_timer_get_time();
        if (end-start<10) {
            ESP_LOGE(TAG,"%s:%d timeout for bit %d", __FILE__,__LINE__, i);
            return Dht11Result::INVALID_TIMEOUT;
        }
        if (end-start>50) {
            data[dataIndex] |= 0x01;
        }
        if (bitIndex<7){
            data[dataIndex] = data[dataIndex] << 1;
            bitIndex++;
        } else {
            bitIndex=0;
            dataIndex++;
        }
    }
    ESP_LOGI(TAG,"Read 0x%02X, 0x%02X,0x%02X,0x%02X,0x%02X", (uint16_t )data[0],(uint16_t )data[1], (uint16_t )data[2], (uint16_t )data[3], (uint16_t )data[4]  );
    if (checkParity(data) != Dht11Result::VALID){
        ESP_LOGE(TAG, "Parity error");
        return Dht11Result::PARITY_ERROR;
    }
    humidity = data[0]+data[1]/255.0;
    temp = data[2]+data[3]/255.0;
    ESP_LOGI(TAG, "Temp: %f, Humidity: %f", temp, humidity);
    return Dht11Result::VALID;
}

void Dht11::start() {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(20 * 1000);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    ets_delay_us(40);

}

void Dht11::init(SensorSetting &setting) {
    name = setting.name;
    pin = setting.pins[0];
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = BIT(pin)  ;
    gpioConfig.mode = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpioConfig);

    temp=0;
    humidity=0;

    confTemp = make_unique<MqttSensorConfigurationMessage>(name, TEMP_SENSOR_NAME, "°C", "mdi:thermometer");
    confHumidity = make_unique<MqttSensorConfigurationMessage>(name, HUMIDITY_SENSOR_NAME, "%", nullptr);

}

Dht11Result Dht11::waitFor1(uint8_t timeout) {
    uint8_t micros_ticks = 0;
    while(gpio_get_level(pin) == 0 && micros_ticks < timeout) {
        micros_ticks++;
        ets_delay_us(1);
    }
    if (micros_ticks >= timeout)
        return Dht11Result::INVALID_TIMEOUT;
    else
        return Dht11Result::VALID;
}


Dht11Result Dht11::waitFor0(uint8_t timeout) {
    uint8_t micros_ticks = 0;
    while(gpio_get_level(pin) == 1 && micros_ticks < timeout) {
        micros_ticks++;
        ets_delay_us(1);
    }
    if (micros_ticks >= timeout)
        return Dht11Result::INVALID_TIMEOUT;
    else
        return Dht11Result::VALID;
}

Dht11Result Dht11::checkParity(const uint8_t *data) {
    if( data[4] == ((data[0] + data[1] + data[2] + data[3])&0xFF))
        return Dht11Result::VALID;
    else {
        uint8_t a = (data[0] + data[1] + data[2] + data[3]);
        ESP_LOGE(TAG, "%d != %d", a, data[4]);
        return Dht11Result::PARITY_ERROR;
    }
}

void Dht11::step() {
    if(read() == Dht11Result::VALID){
        ESP_LOGI(TAG,"Temp: %f, humidity: %f", temp, humidity);
    } else {
        ESP_LOGI(TAG,"INVAliD");
    }

}

std::vector<std::unique_ptr<AutoconfigurationTopic>> Dht11::autoconfigure() {
    auto jsons = vector<std::unique_ptr<AutoconfigurationTopic>>();

    jsons.push_back(make_unique<AutoconfigurationTopic>(confTemp->createConfig(), confTemp->createConfigTopicName()));
    jsons.push_back(make_unique<AutoconfigurationTopic>(confHumidity->createConfig(), confHumidity->createConfigTopicName()));

    return jsons;

}

void Dht11::setState(cJSON *state) {
    char buffer[100];
    read();
    sprintf(buffer, "%f", temp);
    cJSON_AddStringToObject(state, confTemp->stateFieldName, buffer);
    sprintf(buffer, "%f", humidity);
    cJSON_AddStringToObject(state, confHumidity->stateFieldName, buffer);
}

std::vector<std::unique_ptr<SubscribingTopic>> Dht11::subscribingTopics() {
    return std::vector<std::unique_ptr<SubscribingTopic>>();
}

