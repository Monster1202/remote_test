#pragma once

#include "esp_err.h"

//#define __cplusplus

#ifdef __cplusplus
extern "C" {

#endif

void http_test_task(void *pvParameters);  //
void lcd_draw(void *pvParameters);

void frame_send(int y_lineindex,uint8_t *send_buffer);

esp_err_t test_app(void);
// void FTC533_cycle(void);
// void FTC533_process(void);
#ifdef __cplusplus
}
#endif
