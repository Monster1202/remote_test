#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//esp_err_t test_app(void);
void wifi_connect(void);
int8_t get_rssi(void);
void wifi_reset(void);
void wifi_scan(void);

#ifdef __cplusplus
}
#endif
