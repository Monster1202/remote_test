//#pragma once
#ifndef __PARA_LIST_H__
#define __PARA_LIST_H__

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_TYPE_REMOTE 


#define BACKUP_MQTT_BROKER_URL     "mqtt://10.42.0.1"    //"mqtt://172.16.170.48"  //
#define BACKUP_EXAMPLE_ESP_WIFI_SSID       "CLEANING-SYSTEM" //"yyg" // "SHKJ2020" //  //
#define BACKUP_EXAMPLE_ESP_WIFI_PASS      "12345678"     //  "shkj1234." //
#define http_get_URL    "http://10.42.0.1:8090/test.mjpg"//"http://10.42.0.1:8080/?action=stream"////"http://172.16.170.189:8090/test.mjpg" // "http://172.16.170.189:8080/?action=stream" //  "http://172.16.170.48:8080/?action=stream"  ////"http://172.16.170.189:8080/stream?topic=/usb_cam/image_raw",

typedef struct
{
    char wifi_ssid[32];
    char wifi_pass[64];
    char broker_url[60];
    char update_url[60];
}PARAMETER_CONNECTION;


// #define GPIO_SYS_LED         0
// #define GPIO_BEEP            8

// #ifdef DEVICE_TYPE_REMOTE
// #define CONFIG_EXAMPLE_FIRMWARE_UPG_URL "http://172.16.171.221:8070/remote.bin"
// #define ECHO_TEST_TXD   (9)
// #define ECHO_TEST_RXD   (10)

// #define GPIO_INPUT_IO_1     35
// #define GPIO_INPUT_IO_2     36
// #define GPIO_INPUT_IO_3     37
// #define GPIO_INPUT_IO_4     38
// #define GPIO_INPUT_IO_5     39
// #define GPIO_INPUT_IO_6     40
// #define GPIO_INPUT_IO_7     41
// #define GPIO_INPUT_IO_STOP     42//stop
// #define GPIO_OUTPUT_LED_1         18
// #define GPIO_OUTPUT_LED_2         17
// #define GPIO_OUTPUT_LED_3         16
// #define GPIO_OUTPUT_LED_4         15
// #define GPIO_OUTPUT_LED_5         7
// #define GPIO_OUTPUT_LED_6         6
// #define GPIO_SYS_LED         0    //stop
// #define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_SYS_LED) |(1ULL<<GPIO_BEEP) | (1ULL<<GPIO_OUTPUT_LED_1)| (1ULL<<GPIO_OUTPUT_LED_2)| (1ULL<<GPIO_OUTPUT_LED_3)| (1ULL<<GPIO_OUTPUT_LED_4)| (1ULL<<GPIO_OUTPUT_LED_5)| (1ULL<<GPIO_OUTPUT_LED_6))  
// #define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_1)|(1ULL<<GPIO_INPUT_IO_2)|(1ULL<<GPIO_INPUT_IO_3)|(1ULL<<GPIO_INPUT_IO_4)|(1ULL<<GPIO_INPUT_IO_5)|(1ULL<<GPIO_INPUT_IO_6)|(1ULL<<GPIO_INPUT_IO_7)|(1ULL<<GPIO_INPUT_IO_STOP))
// #define mqtt_test
// #endif

#define CONFIG_EXAMPLE_FIRMWARE_UPG_URL "http://172.16.171.221:8070/remote.bin"
#define MQTT_BROKER_URL     "mqtt://10.42.0.1"
#define EXAMPLE_ESP_WIFI_SSID      "CLEANING-SYSTEM"
#define EXAMPLE_ESP_WIFI_PASS      "12345678"


/////////////////////////////////////////////
uint16_t parameter_read_pressure(void);
void parameter_write_pressure(uint16_t pressure);
void parameter_write_temperature(double temperature);
double parameter_read_temperature(void);
void para_init(void);
esp_err_t get_chip_id(uint32_t* chip_id);

void parameter_write_centralizer(uint8_t value);
uint8_t parameter_read_centralizer(void);
void parameter_write_rotation(uint8_t value);
uint8_t parameter_read_rotation(void);
void parameter_write_nozzle(uint8_t value);
uint8_t parameter_read_nozzle(void);
void parameter_write_emergency_stop(uint8_t value);
uint8_t parameter_read_emergency_stop(void);

void parameter_write_msg_id(char *str_msgid);
char *parameter_read_msg_id(void);
void parameter_write_timestamp(double timestamp);
double parameter_read_timestamp(void);
void parameter_write_water(uint8_t value);
uint8_t parameter_read_water(void);
void parameter_write_pressure_alarm(uint8_t value);
uint8_t parameter_read_pressure_alarm(void);

void parameter_write_heater(uint8_t value);
uint8_t parameter_read_heater(void);
void parameter_write_mode(uint8_t value);
uint8_t parameter_read_mode(void);
double blister_read_temperature(void);
void parameter_write_liquid_alarm(uint8_t value);

typedef struct
{
    char uuid[6];
    uint8_t nozzle;      //command down
    uint8_t centralizer;
    uint8_t rotation;
    uint8_t status;     //status upload
    uint8_t water;
    uint8_t pressure_alarm;
    uint8_t emergency_stop;
    double timestamp;
    char msg_id[30];
    double temperature;
    uint16_t pressure;
    char version[30];
    int8_t rssi;
    uint8_t wifi_connection;    
    uint8_t air_pump;
    int time_int;
    char time_string[20];
}PARAMETER_BRUSH;

typedef struct
{
    char uuid[6];
    uint8_t mode;      //command down
    uint8_t heater;    
    uint8_t status;     //status upload
    uint8_t water;
    uint8_t pressure_alarm;
    uint8_t liquid_alarm;
    uint8_t emergency_stop;
    double timestamp;
    char msg_id[30];
    double temperature;
    uint16_t pressure;
    char version[30];
    int8_t rssi;
    uint8_t wifi_connection; 
    uint8_t air_pump;
    int time_int;
    char time_string[20];
}PARAMETER_BLISTER;

typedef struct
{
    char uuid[6];
    uint8_t status;     //status upload
    double timestamp;
    char msg_id[30];
    uint8_t nozzle;      //command down
    uint8_t centralizer;
    uint8_t rotation;
    uint8_t water;
    uint8_t pressure_alarm;
    uint8_t heater;
    uint8_t mode;
    uint8_t angle;
    uint8_t emergency_stop;
    char version[30];
    int8_t rssi;
    uint8_t wifi_connection; 
    uint8_t air_pump;
    int time_int;
    char time_string[20];
    uint16_t battery;
    uint8_t sta_brush;
}PARAMETER_REMOTE;

void parameter_write_version(char *str_version);
void get_parameter(PARAMETER_BRUSH *brush_t);
void get_blister_parameter(PARAMETER_BLISTER *blister_t);
void get_remote_parameter(PARAMETER_REMOTE *remote_t);
void parameter_write_rssi(int8_t value);

void parameter_write_wifi_connection(uint8_t value);
uint8_t parameter_read_wifi_connection(void);

void parameter_write_wifi_ssid(char *str_para);

char *parameter_read_wifi_ssid(void);

void parameter_write_wifi_pass(char *str_para);

char *parameter_read_wifi_pass(void);

void parameter_write_broker_url(char *str_para);
char *parameter_read_broker_url(void);

void parameter_write_update_url(char *str_para);
char *parameter_read_update_url(void);
int8_t flash_write_parameter(void);
int8_t flash_read_parameter(void);
void wifi_url_inital_set_para(void);
void wifi1_url_inital_set_para(void);
int8_t flash_erase_parameter(void);


void parameter_write_FTC533(uint8_t value);
uint8_t parameter_read_FTC533(void);
//uint8_t FTC533_KEY_press;
void parameter_write_debug(uint32_t value);
uint32_t parameter_read_debug(void);

void parameter_write_air_pump(uint8_t value);
uint8_t parameter_read_air_pump(void);

void parameter_write_battery(uint16_t value);
uint16_t parameter_read_battery(void);
void parameter_write_sta_brush(uint8_t value);
uint8_t parameter_read_sta_brush(void);

void parameter_write_refresh(uint8_t value);
uint8_t parameter_read_refresh(void);

#ifdef __cplusplus
}
#endif

#endif

