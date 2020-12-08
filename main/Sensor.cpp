//
// Created by paolo on 12/5/20.
//

#include <esp_log.h>
#include "include/Sensor.h"
#include "include/Settings.h"
#include "sensor/include/Dht11.h"
#include "sensor/include/Bistable.h"

Sensor * * sensors;
uint8_t totSensors;
static const char * TAG="Sensor";

static std::unique_ptr<Sensor> makeSensor(SensorType type){
    switch (type) {
        case SensorType::DHT11:
            ESP_LOGI(TAG, "Creating dht11 sensor");
            return std::make_unique<Dht11>();
        case SensorType::BISTABLE:
            ESP_LOGI(TAG, "Creating Bistable sensor");
            return std::make_unique<Bistable>();
            break;
    }
    return std::unique_ptr<Sensor>();
}

void createSensors() {
    ESP_LOGI(TAG,"Creating sensors");
    auto settings = Settings::settings.sensorSettings.get();
    sensors = new Sensor *[Settings::settings.totSensors];
    totSensors = Settings::settings.totSensors;

    for(uint8_t i=0; i<Settings::settings.totSensors;i++ ){
        auto & sensorSetting = settings[i];
        auto sensor = makeSensor(sensorSetting.sensorType);
        sensor->init(sensorSetting);
        sensors[i] = sensor.release();
    }
}


void stepSensor() {
    ESP_LOGI(TAG,"Step sensor");
    for(uint8_t i=0; i<totSensors; i++){
        sensors[i]->step();
    }
}


