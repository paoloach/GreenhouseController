//
// Created by paolo on 12/2/20.
//

#ifndef GREENHOUSECONTROLLER_SETTINGS_H
#define GREENHOUSECONTROLLER_SETTINGS_H


#ifdef __cplusplus
class Settings {
public:
    static Settings  settings;

    void init();
    char  wifiSSID[32];
    char  wifiPasswd[64];

  };

extern "C" void initSettings();
#else
  void initSettings();
#endif
#endif //GREENHOUSECONTROLLER_SETTINGS_H
