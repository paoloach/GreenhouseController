//
// Created by paolo on 12/2/20.
//

#include <esp32/rom/ets_sys.h>
#include <esp_timer.h>
#include <soc_log.h>
#include <cstring>
#include "include/Dht11.h"

#include "include/SensorSetting.h"
#include "include/Settings.h"

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
    ESP_LOGI(TAG,"Read 0x%02d, 0x%02d,0x%02d,0x%02d,0x%02d", (uint16_t )data[0],(uint16_t )data[1], (uint16_t )data[2], (uint16_t )data[3], (uint16_t )data[4]  );
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
    if(data[4] == (data[0] + data[1] + data[2] + data[3]))
        return Dht11Result::VALID;
    else
        return Dht11Result::PARITY_ERROR;
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

    jsons.push_back(make_unique<AutoconfigurationTopic>(createTempConfiguration(), createTempTopicConfiguration()));
    jsons.push_back(make_unique<AutoconfigurationTopic>(createHumidityConfiguration(), createHumidityTopicConfiguration()));

    return jsons;

}

cJSON *Dht11::createTempConfiguration() {
    cJSON * tempJson = cJSON_CreateObject();
    char buffer[200];

    sprintf(buffer, "%s-%s-%s", Settings::settings.name, name, TEMP_SENSOR_NAME);

    cJSON_AddStringToObject(tempJson, "name", buffer);
    sprintf(buffer, "%s-%s-%s-%s", Settings::settings.name, Settings::settings.strMac, name, TEMP_SENSOR_NAME );
    cJSON_AddStringToObject(tempJson, "uniq_id", buffer);
    cJSON * devJson = cJSON_CreateObject();
    sprintf(buffer, "%s-%s", Settings::settings.name, Settings::settings.strMac);
    cJSON_AddStringToObject(devJson, "ids",buffer);
    cJSON_AddItemToObject(tempJson, "dev",devJson);

    sprintf(buffer, "homeassistant/sensor/%s-%s/state", Settings::settings.name, Settings::settings.strMac);
    cJSON_AddStringToObject(tempJson, "stat_t", buffer);
    cJSON_AddStringToObject(tempJson, "unit_of_meas", "°C");
    cJSON_AddStringToObject(tempJson, "ic", "mdi:thermometer");
    cJSON_AddStringToObject(tempJson, "frc_upd", "false");
    sprintf(buffer, "{{value_json.%s-%s}}", name, TEMP_SENSOR_NAME);
    cJSON_AddStringToObject(tempJson, "val_tpl", buffer);

    return tempJson;
}

char *Dht11::createTempTopicConfiguration() {
    char buffer[200];

    sprintf(buffer, "homeassistant/sensor/%s-%s-%s-%s/config", Settings::settings.name, Settings::settings.strMac, name,TEMP_SENSOR_NAME);
    return strdup(buffer);
}

cJSON *Dht11::createHumidityConfiguration() {
    cJSON * humidityJson = cJSON_CreateObject();
    char buffer[200];

    sprintf(buffer, "%s-%s-%s", Settings::settings.name, name, HUMIDITY_SENSOR_NAME);

    cJSON_AddStringToObject(humidityJson, "name", buffer);
    sprintf(buffer, "%s-%s-%s-%s", Settings::settings.name, Settings::settings.strMac, name, HUMIDITY_SENSOR_NAME );
    cJSON_AddStringToObject(humidityJson, "uniq_id", buffer);
    cJSON * devJson = cJSON_CreateObject();
    sprintf(buffer, "%s-%s", Settings::settings.name, Settings::settings.strMac);
    cJSON_AddStringToObject(devJson, "ids",buffer);
    cJSON_AddItemToObject(humidityJson, "dev",devJson);

    sprintf(buffer, "homeassistant/sensor/%s-%s/state", Settings::settings.name, Settings::settings.strMac);
    cJSON_AddStringToObject(humidityJson, "stat_t", buffer);
    cJSON_AddStringToObject(humidityJson, "unit_of_meas", "%");
    cJSON_AddStringToObject(humidityJson, "frc_upd", "false");
    sprintf(buffer, "{{value_json.%s-%s}}", name, HUMIDITY_SENSOR_NAME);
    cJSON_AddStringToObject(humidityJson, "val_tpl", buffer);

    return humidityJson;
}

char *Dht11::createHumidityTopicConfiguration() {
    char buffer[200];

    sprintf(buffer, "homeassistant/sensor/%s-%s-%s-%s/config", Settings::settings.name, Settings::settings.strMac, name,HUMIDITY_SENSOR_NAME);
    return strdup(buffer);
}

