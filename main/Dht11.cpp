//
// Created by paolo on 12/2/20.
//

#include <esp32/rom/ets_sys.h>
#include <esp_timer.h>
#include <soc_log.h>
#include "Dht11.h"

static const char *TAG = "DHT11";

Dht11Result Dht11::read() {
    start();
    if (waitFor0(80) == Dht11Result::INVALID_TIMEOUT){
        return Dht11Result::INVALID_TIMEOUT;
    }
    if (waitFor1(100) == Dht11Result::INVALID_TIMEOUT){
        return Dht11Result::INVALID_TIMEOUT;
    }
    if (waitFor0(80) == Dht11Result::INVALID_TIMEOUT){
        return Dht11Result::INVALID_TIMEOUT;
    }
    /* Read response */
    uint8_t data[5];
    uint8_t dataIndex=0;
    uint8_t bitIndex=0;
    for(int i = 0; i < 40; i++) {
        if (waitFor1(80) == Dht11Result::INVALID_TIMEOUT){
            ESP_LOGE(TAG, "timeout error at bit %d", i);
            return Dht11Result::INVALID_TIMEOUT;
        }
        int64_t start = esp_timer_get_time();
        if (waitFor1(100) == Dht11Result::INVALID_TIMEOUT){
            ESP_LOGE(TAG, "timeout error at bit %d", i);
            return Dht11Result::INVALID_TIMEOUT;
        }
        int64_t end = esp_timer_get_time();
        if (end-start<10)
            return Dht11Result::INVALID_TIMEOUT;
        if (end-start>50) {
            data[dataIndex] |= 0x01;
        }
        if (bitIndex<7){
            data[dataIndex] = data[dataIndex] << 1;
            bitIndex++;
        } else {
            bitIndex=0;
            dataIndex++;
        }
    }
    ESP_LOGI(TAG,"Read 0x%02d, 0x%02d,0x%02d,0x%02d,0x%02d", (uint16_t )data[0],(uint16_t )data[1], (uint16_t )data[2], (uint16_t )data[3], (uint16_t )data[4]  );
    if (checkParity(data) != Dht11Result::VALID){
        ESP_LOGE(TAG, "Parity error");
        return Dht11Result::PARITY_ERROR;
    }
    humidity = data[0]+data[1]/10.0;
    temp = data[2]+data[3]/10.0;
    ESP_LOGI(TAG, "Temp: %f, Humidity: %f", temp, humidity);
    return Dht11Result::VALID;
}

void Dht11::start() {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(20 * 1000);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    ets_delay_us(40);

}

Dht11::Dht11(gpio_num_t pin) :pin(pin){
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = BIT(pin)  ;
    gpioConfig.mode = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpioConfig);

    temp=0;
    humidity=0;
}

Dht11Result Dht11::waitFor1(uint8_t timeout) {
    uint8_t micros_ticks = 0;
    while(gpio_get_level(pin) == 0 && micros_ticks < timeout) {
        micros_ticks++;
        ets_delay_us(1);
    }
    if (micros_ticks >= timeout)
        return Dht11Result::INVALID_TIMEOUT;
    else
        return Dht11Result::VALID;
}


Dht11Result Dht11::waitFor0(uint8_t timeout) {
    uint8_t micros_ticks = 0;
    while(gpio_get_level(pin) == 1 && micros_ticks < timeout) {
        micros_ticks++;
        ets_delay_us(1);
    }
    if (micros_ticks >= timeout)
        return Dht11Result::INVALID_TIMEOUT;
    else
        return Dht11Result::VALID;
}

Dht11Result Dht11::checkParity(const uint8_t *data) {
    if(data[4] == (data[0] + data[1] + data[2] + data[3]))
        return Dht11Result::VALID;
    else
        return Dht11Result::PARITY_ERROR;
}
