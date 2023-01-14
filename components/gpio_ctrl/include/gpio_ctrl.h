#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//gpio
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "para_list.h"


#define DEVICE_TYPE_REMOTE


void sw_key_read(uint8_t io_num,uint8_t state);
uint8_t KEY_READ(uint8_t io_num);
void gpio_init(void);
uint8_t UI_press_output(uint8_t value,uint8_t button);


#ifdef DEVICE_TYPE_REMOTE
void remote_stop_io_out(uint8_t value , uint8_t state);
void remote_press_output(uint8_t io_num);
void centralizer_io_out(uint8_t value);
void rotation_io_out(uint8_t value);
void nozzle_io_out(uint8_t value);
#endif

void heater_init(void);
uint8_t tca9554_ext_io_read(void);
#ifdef __cplusplus
}
#endif
