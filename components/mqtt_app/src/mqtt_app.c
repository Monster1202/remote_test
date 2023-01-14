#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_app.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "para_list.h"
#include "gpio_ctrl.h"
#include "wifi_sta.h"



#define TOPIC_TIMESTAMP "/timestamp"
#define TOPIC_EMERGENCY_CONTROL "/emergency-control"
#define TOPIC_DEVICE_REGISTER "/device-register"

#define TOPIC_BRUSH_CONTROL "/pneumatic-brush-device/switch-control"
#define TOPIC_BRUSH_STATES "/pneumatic-brush-device/states"
#define TOPIC_BLISTER_CONTROL "/blister-device/switch-control"
#define TOPIC_BLISTER_STATES "/blister-device/states"
#define TOPIC_REMOTE_CONTROL "/remote-control-device/switch-control"
#define TOPIC_REMOTE_STATES "/remote-control-device/states"
//PARAMETER_BRUSH brush_para;
#define DEVICE_TYPE_REMOTE

//char data_pub_1[500] = {0};
static const char *TAG = "MQTT_EXAMPLE";
esp_mqtt_client_handle_t mqtt_client;
static uint16_t buf_disconnect = 0;

uint8_t flag_write_para = 0;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void flashwrite_reset(void)
{
    if(flash_write_parameter() == -1)
        ESP_LOGI(TAG, "flash_write_parameter_error!");
    if(flag_write_para == 0x01 || flag_write_para == 0x03){
        wifi_reset();
    }
    if(flag_write_para == 0x02 || flag_write_para == 0x03){
        vTaskDelay(1000 / portTICK_RATE_MS);
        mqtt_reset();
    }
    flag_write_para = 0;
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
     
    char data_pub_1[300] = "init";
    //memset(data_pub_1,0,sizeof(data_pub_1));
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        parameter_write_wifi_connection(3);
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        msg_id = esp_mqtt_client_subscribe(client, TOPIC_TIMESTAMP, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        

            data_publish(data_pub_1,4);   //device_register
            msg_id = esp_mqtt_client_publish(client, TOPIC_DEVICE_REGISTER, data_pub_1, 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, TOPIC_BRUSH_STATES, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            // msg_id = esp_mqtt_client_subscribe(client, TOPIC_BLISTER_STATES, 0);
            // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        break;
    case MQTT_EVENT_DISCONNECTED:
        parameter_write_wifi_connection(2);
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED buf_disconnect=%d",buf_disconnect);
        #ifndef DEVICE_TYPE_BLISTER  
        buf_disconnect++;            
        if(buf_disconnect == 15)   //10=1minute
            esp_restart();
        #endif   
        // uint8_t wifi_sta = 0;
        // wifi_sta=parameter_read_wifi_connection();
        // ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED wifi_sta=%d",wifi_sta);
        // if(wifi_sta)//wifi connected 
        //     esp_mqtt_client_reconnect(mqtt_client);
        // if(buf_disconnect == 10)
        // {
        //     wifi_reset();
        //     mqtt_reset();
        //     buf_disconnect = 0;
        // }    
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, topic_sub_1, "SUBSCRIBED", 0, 0, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG, "TOPIC=%.*s\r", event->topic_len, event->topic);
        ESP_LOGI(TAG, "DATA=%.*s\r", event->data_len, event->data);
        data_process(event->data);

#ifdef DEVICE_TYPE_REMOTE     
        //char *str_topic = "/timestamp";
        if(event->topic_len<=12){
            data_publish(data_pub_1,9); 
            msg_id = esp_mqtt_client_publish(client, TOPIC_REMOTE_STATES, data_pub_1, 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        }
#endif 
        buf_disconnect = 0;
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    if(flag_write_para)
        flashwrite_reset();
}

void mqtt_app_start(void)
{
    char *broker_url = {0};
    broker_url = parameter_read_broker_url();
    printf("parameter_read_broker_url:%s",broker_url); 

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = broker_url,// MQTT_BROKER_URL,//CONFIG_BROKER_URL,  
        .task_prio = 5,//MQTT_PRIO,
    };
    //strcpy(mqtt_cfg.uri,broker_url);
    //esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    // esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    // esp_mqtt_client_start(client);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}



void mqtt_init(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);//  CONFIG_LOG_COLORS  ESP_LOG_ERROR
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_ERROR);////ESP_LOG_DEBUG ESP_LOG_INFO ESP_LOG_WARN
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    //vTaskDelay(6000 / portTICK_RATE_MS); 
    mqtt_app_start();
}

void data_process(char *data)
{
    cJSON *json_str_xy = cJSON_Parse(data);
    if(json_str_xy == NULL) {
        cJSON_Delete(json_str_xy);
        return 0;
    }
    cJSON *json_brush_water = cJSON_GetObjectItem(json_str_xy, "water");
    if(json_brush_water != NULL && json_brush_water->type == cJSON_Number) {
        ESP_LOGI(TAG, "json_brush_water = %d", json_brush_water->valueint);
        parameter_write_water(json_brush_water->valueint);
    }
    cJSON *json_brush_pressure = cJSON_GetObjectItem(json_str_xy, "pressure_alarm");
    if(json_brush_pressure != NULL && json_brush_pressure->type == cJSON_Number) {
        ESP_LOGI(TAG, "json_brush_pressure = %d", json_brush_pressure->valueint);
        parameter_write_pressure_alarm(json_brush_pressure->valueint);
    }
    cJSON *json_brush_status = cJSON_GetObjectItem(json_str_xy, "status");
    if(json_brush_status != NULL && json_brush_status->type == cJSON_Number) {
        ESP_LOGI(TAG, "status = %d", json_brush_status->valueint);
        parameter_write_sta_brush(json_brush_status->valueint);
    }
    //parameter_write_sta_brush
    cJSON *json_emergency_stop = cJSON_GetObjectItem(json_str_xy, "emergency_stop");
    if(json_emergency_stop != NULL && json_emergency_stop->type == cJSON_Number) {
        ESP_LOGI(TAG, "emergency_stop = %d", json_emergency_stop->valueint);
        remote_stop_io_out(json_emergency_stop->valueint,1);
    }
    uint8_t register_emergency_stop = 0;
    register_emergency_stop = parameter_read_emergency_stop();
    cJSON *json_centralizer = cJSON_GetObjectItem(json_str_xy, "centralizer");
    if(json_centralizer != NULL && json_centralizer->type == cJSON_Number) {
        ESP_LOGI(TAG, "centralizer = %d", json_centralizer->valueint);
        if(register_emergency_stop==0)
            centralizer_io_out(json_centralizer->valueint);
    }
    cJSON *json_rotation = cJSON_GetObjectItem(json_str_xy, "rotation");
    if(json_rotation != NULL && json_rotation->type == cJSON_Number) {
        ESP_LOGI(TAG, "rotation = %d", json_rotation->valueint);
        if(register_emergency_stop==0)
            rotation_io_out(json_rotation->valueint);
    }
    cJSON *json_nozzle = cJSON_GetObjectItem(json_str_xy, "nozzle");
    if(json_nozzle != NULL && json_nozzle->type == cJSON_Number) {
        ESP_LOGI(TAG, "nozzle = %d", json_nozzle->valueint);
        if(register_emergency_stop==0)
            nozzle_io_out(json_nozzle->valueint);
    }

    cJSON *json_timestamp = cJSON_GetObjectItem(json_str_xy, "timestamp");
    if(json_timestamp != NULL && json_timestamp->type == cJSON_Number) {
        //brush_para.timestamp = json_timestamp->valuedouble;
        parameter_write_timestamp(json_timestamp->valuedouble);
        ESP_LOGI(TAG, "timestamp = %f", json_timestamp->valuedouble);
    }
    cJSON *json_msg_id = cJSON_GetObjectItem(json_str_xy, "msg_id");
    if(json_msg_id != NULL && json_msg_id->type == cJSON_String) {
        //strcpy(brush_para.msg_id,json_msg_id->valuestring);
        parameter_write_msg_id(json_msg_id->valuestring);
        ESP_LOGI(TAG, "msg_id = %s", json_msg_id->valuestring);
    }

    cJSON *json_wifi_ssid = cJSON_GetObjectItem(json_str_xy, "wifi_ssid");
    if(json_wifi_ssid != NULL && json_wifi_ssid->type == cJSON_String) {
        parameter_write_wifi_ssid(json_wifi_ssid->valuestring);
        ESP_LOGI(TAG, "wifi_ssid = %s", json_wifi_ssid->valuestring);
        flag_write_para = 1;
    }
    cJSON *json_wifi_pass = cJSON_GetObjectItem(json_str_xy, "wifi_pass");
    if(json_wifi_pass != NULL && json_wifi_pass->type == cJSON_String) {
        parameter_write_wifi_pass(json_wifi_pass->valuestring);
        ESP_LOGI(TAG, "wifi_pass = %s", json_wifi_pass->valuestring);
        flag_write_para = 1;
    }
    cJSON *json_broker_url = cJSON_GetObjectItem(json_str_xy, "broker_url");
    if(json_broker_url != NULL && json_broker_url->type == cJSON_String) {
        parameter_write_broker_url(json_broker_url->valuestring);
        ESP_LOGI(TAG, "broker_url = %s", json_broker_url->valuestring);
        flag_write_para += 2;
    }
    cJSON *json_update_url = cJSON_GetObjectItem(json_str_xy, "update_url");
    if(json_update_url != NULL && json_update_url->type == cJSON_String) {
        parameter_write_update_url(json_update_url->valuestring);
        ESP_LOGI(TAG, "update_url = %s", json_update_url->valuestring);
    }
    cJSON *json_buffer = cJSON_GetObjectItem(json_str_xy, "erase");
    if(json_buffer != NULL && json_buffer->type == cJSON_String) {
        ESP_LOGI(TAG, "erase : %s", json_buffer->valuestring);
        if(flash_erase_parameter() == -1)
            printf("flash_erase_parameter error");
    }
    cJSON *json_debug_parameter = cJSON_GetObjectItem(json_str_xy, "debug_parameter");
    if(json_debug_parameter != NULL && json_debug_parameter->type == cJSON_Number) {
        ESP_LOGI(TAG, "debug_parameter = %d", json_debug_parameter->valueint);
        parameter_write_debug(json_debug_parameter->valueint);
    }
    // if(flag_write_para)
    //     flashwrite_reset();
    cJSON_Delete(json_str_xy);
}



void device_states_publish(uint8_t button)
{
    int msg_id = 0;
    char data_pub_1[300] = "init";
    //memset(data_pub_1,0,sizeof(data_pub_1));
#ifdef DEVICE_TYPE_REMOTE
    switch(button)
    {
        case 1:data_publish(data_pub_1,5);
        msg_id = esp_mqtt_client_publish(mqtt_client, TOPIC_BRUSH_CONTROL, data_pub_1, 0, 1, 0);break;
        case 2:data_publish(data_pub_1,6);
        msg_id = esp_mqtt_client_publish(mqtt_client, TOPIC_BRUSH_CONTROL, data_pub_1, 0, 1, 0);break;
        case 3:data_publish(data_pub_1,7);
        msg_id = esp_mqtt_client_publish(mqtt_client, TOPIC_BRUSH_CONTROL, data_pub_1, 0, 1, 0);break;
        case 4:data_publish(data_pub_1,8);
        msg_id = esp_mqtt_client_publish(mqtt_client, TOPIC_EMERGENCY_CONTROL, data_pub_1, 0, 1, 0);break;
        default:break;
    }   
#endif    
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
}


void data_publish(char *data,uint8_t case_pub)
{

    PARAMETER_REMOTE remote_buf = {0};
    get_remote_parameter(&remote_buf);

    cJSON *root = cJSON_CreateObject();

    if(case_pub == 4){
        char string_uuid[20] = "0";//7cdfa1e592e0
        hex2str((uint8_t *)remote_buf.uuid,6,string_uuid);
        cJSON_AddItemToObject(root, "device_sn",cJSON_CreateString(string_uuid));
        cJSON_AddNumberToObject(root, "timestamp",remote_buf.timestamp);
        cJSON_AddItemToObject(root, "device_type",cJSON_CreateString("PARAMETER_REMOTE"));
        cJSON_AddItemToObject(root, "device_version",cJSON_CreateString(remote_buf.version));
        }
    else if(case_pub == 5){
        cJSON_AddItemToObject(root, "switch_name",cJSON_CreateString("centralizer"));
        cJSON_AddNumberToObject(root, "value",remote_buf.centralizer);
        }
    else if(case_pub == 6){
        cJSON_AddItemToObject(root, "switch_name",cJSON_CreateString("rotation"));
        cJSON_AddNumberToObject(root, "value",remote_buf.rotation);
        }
    else if(case_pub == 7){
        cJSON_AddItemToObject(root, "switch_name",cJSON_CreateString("nozzle"));
        cJSON_AddNumberToObject(root, "value",remote_buf.nozzle);
        }
    else if(case_pub == 8){
        cJSON_AddNumberToObject(root, "emergency_stop",1);//remote_buf.emergency_stop
        }
    else if(case_pub == 9){
        cJSON_AddNumberToObject(root, "status",remote_buf.status);
        cJSON_AddNumberToObject(root, "rssi",remote_buf.rssi);
        cJSON_AddNumberToObject(root, "battery",remote_buf.battery);
        }

    char *msg = cJSON_Print(root);
    ESP_LOGI(TAG, "%s",msg); 
    strcpy(data,msg);
    free(msg);
    cJSON_Delete(root);
}

void mqtt_reset(void)
{
    char *broker_url = {0};
    broker_url = parameter_read_broker_url();
    printf("parameter_read_broker_url:%s",broker_url); 

    esp_mqtt_client_stop(mqtt_client);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_set_uri(mqtt_client,broker_url);

    esp_mqtt_client_start(mqtt_client);
}


void hex2str(uint8_t *input, uint16_t input_len, char *output)
{
    char *hexEncode = "0123456789ABCDEF";
    int i = 0, j = 0;

    for (i = 0; i < input_len; i++) {
        output[j++] = hexEncode[(input[i] >> 4) & 0xf];
        output[j++] = hexEncode[(input[i]) & 0xf];
    }
}

#define isDigit(c)             (((c) <= '9' && (c) >= '0') ? (1) : (0))

static uint8_t hex2dec(char hex)
{
    if (isDigit(hex)) {
        return (hex - '0');
    }
    if (hex >= 'a' && hex <= 'f') {
        return (hex - 'a' + 10);
    }
    if (hex >= 'A' && hex <= 'F') {
        return (hex - 'A' + 10);
    }

    return 0;
}

static int str2hex(char *input, int input_len, unsigned char *output, int max_len)
{
    int             i = 0;
    uint8_t         ch0, ch1;

    if (input_len % 2 != 0) {
        return -1;
    }

    while (i < input_len / 2 && i < max_len) {
        ch0 = hex2dec((char)input[2 * i]);
        ch1 = hex2dec((char)input[2 * i + 1]);
        output[i] = (ch0 << 4 | ch1);
        i++;
    }
    return i;
}

