#ifndef _LCD_ICON_H_
#define _LCD_ICON_H_

#include "esp_err.h"
//#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"



#ifdef __cplusplus
extern "C" {
#endif

void jpg_icon_draw(void *jpg_b,void *lcd_b,size_t i,uint32_t x_0,uint32_t y_0,uint32_t width,uint32_t hight);
void lcd_icon_task(void);
void spiff_init(void);
void lcd_clear_task(void);
#ifdef __cplusplus
}
#endif

#endif

