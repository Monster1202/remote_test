#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void mqtt_init(void);
void data_process(char *data);
void data_publish(char *data,uint8_t case_pub);
void device_states_publish(uint8_t button);

void mqtt_app_start(void);
void mqtt_reset(void);
void hex2str(uint8_t *input, uint16_t input_len, char *output);
static int str2hex(char *input, int input_len, unsigned char *output, int max_len);
static uint8_t hex2dec(char hex);
#ifdef __cplusplus
}
#endif
