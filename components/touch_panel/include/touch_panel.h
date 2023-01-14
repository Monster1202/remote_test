#ifndef _TOUCH_PANEL_H_
#define _TOUCH_PANEL_H_

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

typedef struct{
    int16_t x;
    int16_t y;
    int16_t state;
}touch_t;

static void touchpad_init(void);
uint8_t touchpad_read(touch_t *data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(int16_t * x, int16_t * y);
void touch_input(void);

#ifdef __cplusplus
}
#endif

#endif

