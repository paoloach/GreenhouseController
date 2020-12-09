//
// Created by paolo on 12/2/20.
//

#ifndef GREENHOUSECONTROLLER_SETTINGS_H
#define GREENHOUSECONTROLLER_SETTINGS_H


#ifdef __cplusplus

#include <memory>
#include "SensorSetting.h"

class Settings {
public:
    static Settings  settings;


    void init();
    char name[32];
    char strMac[7];
    char wifiSSID[32];
    char wifiPasswd[64];
    char mqttBrokerUrl[32];
    char mqttUsername[32];
    char mqttPassword[32];
    cJSON * devDescription();
    bool mqttEnable;
    std::unique_ptr<SensorSetting[]> sensorSettings;
    uint8_t  totSensors;
    char * mqttStateTopic;
  };

extern "C" {
    void initSettings(void);
    const char * getName(void);
}
#else
  void initSettings();
  const char * getName(void);
#endif
#endif //GREENHOUSECONTROLLER_SETTINGS_H
