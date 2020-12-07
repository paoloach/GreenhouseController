//
// Created by paolo on 12/5/20.
//

#ifndef GREENHOUSECONTROLLER_SENSOR_H
#define GREENHOUSECONTROLLER_SENSOR_H


#ifdef __cplusplus

#include <cstdint>

class SensorSetting;

class Sensor {
public:
    virtual void init(SensorSetting & setting)=0;
    virtual void step() = 0;
};

extern  Sensor * * sensors;
extern  uint8_t totSensors;

extern "C" {
    void createSensors();
    void stepSensor();
}
#else

void createSensors(void);
void stepSensor(void);

#endif

#endif //GREENHOUSECONTROLLER_SENSOR_H
