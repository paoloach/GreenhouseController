#include "esphome.h"

class MoistMCP3201Sensor : public PollingComponent, public Sensor {
 public:
     MoistMCP3201Sensor():PollingComponent(60000){
         
    }
  void setup() override {
    SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    SPI.begin();
  }
  void loop() override {
    // This will be called by App.loop()
  }
};
