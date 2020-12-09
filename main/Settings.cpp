//
// Created by paolo on 12/2/20.
//

#include <driver/gpio.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <cstring>
#include <esp_system.h>
#include "include/Settings.h"
#include "include/Passwords.h"


Settings Settings::settings;

void initSettings() {
    Settings::settings.init();
}

const char * getName(void) {
    return Settings::settings.name;
}

void Settings::init() {
    char buffer[100];
    uint8_t mac[6];
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    sprintf(strMac, "%02X%02X%02X", mac[0],mac[1] ,mac[2] );

    strcpy(name, "serra");
    strcpy(wifiSSID, SSID);
    strcpy(wifiPasswd, PASSWORD);
    strcpy(mqttBrokerUrl, MQTT_URL);
    strcpy(mqttUsername, MQTT_USER);
    strcpy(mqttPassword, MQTT_PASS);
    sprintf(buffer, "homeassistant/sensor/%s-%s/state", name, strMac);
    mqttStateTopic = strdup(buffer);

    sensorSettings = std::make_unique<SensorSetting[]>(3);
    sensorSettings[0].name="Global";
    sensorSettings[0].sensorType = SensorType::DHT11;
    sensorSettings[0].pins = std::make_unique<gpio_num_t[]>(1);
    sensorSettings[0].pins[0] = GPIO_NUM_25;

    sensorSettings[1].name="Light";
    sensorSettings[1].sensorType = SensorType::BISTABLE;
    sensorSettings[1].pins = std::make_unique<gpio_num_t[]>(2);
    sensorSettings[1].pins[0] = GPIO_NUM_16;
    sensorSettings[1].pins[1] = GPIO_NUM_17;

    sensorSettings[1].name="Heater";
    sensorSettings[1].sensorType = SensorType::MONOSTABLE;
    sensorSettings[1].pins = std::make_unique<gpio_num_t[]>(1);
    sensorSettings[1].pins[0] = GPIO_NUM_23;

    totSensors=3;
    mqttEnable=true;
}

cJSON *Settings::devDescription() {
    char buffer[200];
    cJSON * devJson = cJSON_CreateObject();
    sprintf(buffer, "%s-%s", Settings::settings.name, Settings::settings.strMac);
    cJSON_AddStringToObject(devJson, "ids",buffer);
    return devJson;
}
