//
// Created by paolo on 12/2/20.
//

#include <driver/gpio.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <cstring>
#include "include/Settings.h"
#include "include/Passwords.h"


Settings Settings::settings;

void initSettings() {
    Settings::settings.init();
}

void Settings::init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    strcpy(wifiSSID, SSID);
    strcpy(wifiPasswd, PASSWORD);
    strcpy(mqttBrokerUrl, MQTT_URL);
    strcpy(mqttUsername, MQTT_USER);
    strcpy(mqttPassword, MQTT_PASS);

    sensorSettings = std::make_unique<SensorSetting[]>(1);
    sensorSettings[0].name="Temperature";
    sensorSettings[0].sensorType = SensorType::DHT11;
    sensorSettings[0].pins = std::make_unique<gpio_num_t[]>(1);
    sensorSettings[0].pins[0] = GPIO_NUM_25;

    totSensors=1;
    mqttEnable=true;
}
