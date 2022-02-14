#include <math.h>
#include <tuple>
#include <functional>
#include <map>
#include "esphome.h"
using namespace esphome;
#include <SPI.h>
#include <fft.h>

#define DATA_SIZE 1024

static const char *const TAG = "MoistMCP3201Sensor";
class MoistMCP3201Sensor : public PollingComponent, public Sensor {
private:
    uint8_t clkPin;
    uint8_t misoPin;
    uint8_t csPin;
    uint16_t data[DATA_SIZE];
    uint16_t data2[DATA_SIZE];
/*    float fftInput[2*DATA_SIZE];
    float fftOut[2*DATA_SIZE];
    float twiddleFactors[2*DATA_SIZE];
    uint16_t  dataIndex;
    */
 public:
     MoistMCP3201Sensor(uint8_t clkPin, uint8_t misoPin, uint8_t csPin ):PollingComponent(59000),
        clkPin(clkPin),
        misoPin(misoPin),
        csPin(csPin)
//        dataIndex(0)
        {
      /*  float two_pi_by_n = TWO_PI / DATA_SIZE;
        uint16_t m=0;
        
            
         for (uint16_t k = 0; k < DATA_SIZE; k++) {
            twiddleFactors[m] = cosf(two_pi_by_n * k);    // real
            m++;
            twiddleFactors[m] = sinf(two_pi_by_n * k);  // imag
            m++;
        }
        */    
      set_state_class(STATE_CLASS_MEASUREMENT);
      set_device_class("humidity");
    }
  void setup() override ;
  void update() override;
  void dump_config() override;
  uint16_t calcMean(uint16_t dataSize) ;
  uint16_t calcError(uint16_t dataSize, uint16_t mean);
  uint16_t read();
  std::tuple<uint16_t, uint16_t> meanError(uint16_t dataSize);
  uint16_t clean(uint16_t dataSize, uint16_t mean, uint16_t error);
};

uint16_t MoistMCP3201Sensor::read() {
    SPI.setBitOrder(SPI_MSBFIRST);
    digitalWrite(csPin, false);
    uint16_t result = SPI.transfer16(0);
    digitalWrite(csPin, true);

    result &= 0x1FFF;
    result >>= 1;
    return result; 
}

std::tuple<uint16_t, uint16_t> MoistMCP3201Sensor::meanError(uint16_t dataSize){
    if (dataSize==0){
        return std::make_tuple(0,0);
    }
    uint16_t mean =  calcMean(dataSize) ;
    uint16_t error = calcError(dataSize, mean);
    ESP_LOGD(TAG,"Mean: %d, error: %d", mean, error);    
    return std::make_tuple(mean, error);
}

uint16_t MoistMCP3201Sensor::clean(uint16_t dataSize, uint16_t mean, uint16_t error){
    uint16_t j=0;
    
    for(uint16_t i=0; i < dataSize; i++){
        if ( abs(data[i] - mean) < 2*error){
            data2[j] = data[i];
            j++;
        }
    }
    
    
    if (j < dataSize){
        dataSize=j;
        memcpy(data, data2, j*sizeof(uint16_t));
    }
    return dataSize;
}


void MoistMCP3201Sensor::update() {
    
    auto start = millis();
    
    
  std::map<uint16_t, uint16_t> dispersione;
    for (uint16_t i=0; i < DATA_SIZE; i++){
        read();
        data[i] = read();
    }

    auto duration = millis() - start;
    ESP_LOGD(TAG, "Duration: %d millis", duration);    
/*    
    int j=0;
    for ( int i = 0; i  < DATA_SIZE; i++) {
        fftInput[j] = data[i];
        j++;
        fftInput[j]=0;
        j++;
    }
    fft(fftInput, fftOut, twiddleFactors, DATA_SIZE);
    duration = millis() - start;
    ESP_LOGD(TAG, "FFT Duration: %d millis", duration);    
    
    
    std::map<float, uint16_t, std::greater<float> > magnitudo;
    
    float data;
    float real;
    float img;
    for (int i=0; i < DATA_SIZE; i++){
        real = fftOut[2*i];
        img = fftOut[2*i+1];
        data = (100*sqrt(real*real+img*img))/(4096*DATA_SIZE);
        magnitudo[data]=i;
    }
    
    int i=0;
    for (auto && d : magnitudo){
        if (i>=1)
            break;
        ESP_LOGD(TAG, "%d -> %f", d.second, d.first);
        i++;
    }
    
    

    duration = millis() - start;
    ESP_LOGD(TAG, "LOG Duration: %d millis", duration);    
    */
    
    auto mean=meanError(DATA_SIZE);
    uint16_t dataSize = DATA_SIZE;
    uint16_t newDataSize = clean(DATA_SIZE, std::get<0>(mean),std::get<1>(mean));
    if (newDataSize < DATA_SIZE){
        mean=meanError(newDataSize);
    }
    duration = millis() - start;
    ESP_LOGD(TAG, "All duration: %d millis", duration);    

        
    publish_state( (100.0*std::get<0>(mean))/4096.0);
}



uint16_t MoistMCP3201Sensor::calcMean(uint16_t dataSize) {
    ESP_LOGD(TAG, "Calc mean using %d data", dataSize);
    uint32_t mean =0;
    for(uint16_t i=0; i < dataSize; i++){
        mean += data[i];
    }
    mean /= dataSize;
    return mean;
}

uint16_t MoistMCP3201Sensor::calcError(uint16_t dataSize, uint16_t mean){
    uint32_t error=0;
    for(uint16_t i=0; i < dataSize; i++){
        error += (data[i]-mean)*(data[i]-mean);
    }
    error = sqrt(error/DATA_SIZE);
    return error;
}

void MoistMCP3201Sensor::dump_config() {
    ESP_LOGCONFIG(TAG,"MoistMCP3201Sensor:");
    ESP_LOGCONFIG(TAG, " SPI CLK: %d", clkPin);
    //LOG_PIN(TAG, clkPin);
    ESP_LOGCONFIG(TAG, " SPI MISO: %d", misoPin);
    // LOG_PIN(TAG, misoPin);
    ESP_LOGCONFIG(TAG, " CS: %d", csPin);
    //SLOG_PIN(TAG, csPin);

}
void MoistMCP3201Sensor::setup() {
        ESP_LOGCONFIG(TAG,"Setting up MoistMCP3201Sensor");
    pinMode(csPin  , OUTPUT); 
    digitalWrite(csPin, true);    
    SPI.begin(clkPin, misoPin,-1, -1);

}
