/**
 * @file adc121.c
 * @brief adc121 driver header file.
 * @version 0.1
 * @date 2021-03-07
 * 
 * @copyright Copyright 2021 Espressif Systems (Shanghai) Co. Ltd.
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *               http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "adc121.h"

/*   adc i2c address        
(0b1010000)   ADC121C027
(0b1010100)   ADC121C021
(0b1010100)   ADC101C021
(0b1010000)   ADC101C027 */
uint8_t ADC_I2C_ADDR[2] = { 0x50 , 0x54};


#define CONVERSION_RESULT_REG       (0b00000000)
#define ALERT_STATUS                (0b00000001)
#define adc121_CONFIGURATION_REG    (0b00000010)
#define LOW_LIMIT_REG               (0b00000011)
#define HIGH_LIMIT_REG              (0b00000100)
#define HYSTERESIS_REG              (0b00000101)
#define LOWEST_CONVERSION_REG       (0b00000110)
#define HIGHEST_CONVERSION_REG      (0b00000111)

static i2c_bus_device_handle_t adc121_handle;

static esp_err_t adc121_read_byte(uint8_t reg_addr, uint8_t *data)
{
    return i2c_bus_read_byte(adc121_handle, reg_addr, data);
}

static esp_err_t adc121_read_bytes(uint8_t reg_addr, size_t data_len, uint8_t *data)
{
    return i2c_bus_read_bytes(adc121_handle, reg_addr, data_len, data);
}

static esp_err_t adc121_write_byte(uint8_t reg_addr, uint8_t data)
{
    return i2c_bus_write_byte(adc121_handle, reg_addr, data);
}

static esp_err_t adc121_write_bytes(uint8_t reg_addr, size_t data_len, uint8_t *data)
{
    return i2c_bus_write_bytes(adc121_handle, reg_addr, data_len, data);
}

esp_err_t adc121_init(void)
{
    esp_err_t ret;

    if (NULL != adc121_handle) {
        return ESP_FAIL;
    }

    bsp_i2c_add_device(&adc121_handle, ADC_I2C_ADDR[0]);
    ret = adc121_config_default();
    if(ESP_OK != ret) 
    {
        bsp_i2c_add_device(&adc121_handle, ADC_I2C_ADDR[1]);
        ret = adc121_config_default();        
    }

    if(ret == ESP_OK) ESP_LOGI("adc", "adc init ok");
    else ESP_LOGI("adc", "adc init fail");
    return ret;
}

esp_err_t adc121_config(adc121_configuration_reg_t *cfg)
{
    return adc121_write_byte(adc121_CONFIGURATION_REG, cfg->val);
}

esp_err_t adc121_config_default(void)
{
    adc121_configuration_reg_t cfg = {
        .alert_flag_en = false,
        .alert_hold = false,
        .alert_pin_en = false,
        .cycle_time = adc121_cycle_time_2048x,
        .polarity = 0,
    };

    return adc121_write_byte(adc121_CONFIGURATION_REG, cfg.val);
}

esp_err_t adc121_get_config(adc121_configuration_reg_t *cfg)
{
    return adc121_read_byte(adc121_CONFIGURATION_REG, cfg);
}

esp_err_t adc121_get_converted_value(uint16_t *value)
{
    uint8_t reg_val[2];
    printf("adc121_read_bytes\r\n");
    adc121_read_bytes(CONVERSION_RESULT_REG, sizeof(uint16_t), reg_val);
    *value = ((reg_val[0] & 0x0F) << 8) + (reg_val[1]);
    printf("value=%d\r\n",*value);
    return ESP_OK;
}
