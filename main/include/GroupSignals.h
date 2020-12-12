//
// Created by paolo on 12/2/20.
//

#ifndef GREENHOUSECONTROLLER_GROUPSIGNALS_H
#define GREENHOUSECONTROLLER_GROUPSIGNALS_H

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

extern EventGroupHandle_t wifi_event_group;

#define  WIFI_CONNECTED_BIT BIT0
#define  WIFI_FAIL_BIT BIT1
#define  SENSOR_CREATED   BIT2

#endif //GREENHOUSECONTROLLER_GROUPSIGNALS_H
