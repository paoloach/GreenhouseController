//
// Created by paolo on 12/2/20.
//

#include <esp_err.h>
#include <nvs_flash.h>
#include <cstring>
#include "Settings.h"
#include "Passwords.h"


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

}
