#ifndef _KEY_H_
#define _KEY_H_

#include "esp_err.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>
#include "bsp_board.h"

esp_err_t key_init(void);
int key_get_value(void);

esp_err_t key_init_isr(void);

#endif

