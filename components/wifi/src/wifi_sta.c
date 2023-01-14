#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi_sta.h"
#include "para_list.h"
/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID "yyg"//"SHKJ2020"   //CONFIG_ESP_WIFI_SSID  ////"CLEANING-SYSTEM"//
#define EXAMPLE_ESP_WIFI_PASS "12345678" //"shkj1234."  //CONFIG_ESP_WIFI_PASSWORD  // "12345678"//

#define EXAMPLE_ESP_MAXIMUM_RETRY  150//CONFIG_ESP_MAXIMUM_RETRY

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

esp_event_handler_instance_t instance_any_id;
esp_event_handler_instance_t instance_got_ip;
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
// CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE 
// CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE
static const char *TAG = "wifi station";
static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        parameter_write_wifi_connection(0);
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } 
        else if(s_retry_num >= EXAMPLE_ESP_MAXIMUM_RETRY){
            //wifi_reset();
            esp_restart();
            ESP_LOGI(TAG, "reset STA");
            s_retry_num = 1;
        }
        else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
        
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        parameter_write_wifi_connection(1);
    }
    else{
        if(event_base == IP_EVENT)
            ESP_LOGI(TAG, "IP_EVENT,event_id:%d",event_id);
        else if(event_base == WIFI_EVENT)
            ESP_LOGI(TAG, "WIFI_EVENT,event_id:%d",event_id);
    }
}

void wifi_init_sta(void)
{
    uint8_t test_ssid[32] = {0};
    char *wifi_ssid = {0};
    char *wifi_pass = {0};
    wifi_ssid = parameter_read_wifi_ssid();
    wifi_pass = parameter_read_wifi_pass();
    ESP_LOGI(TAG, "wifi_ssid:%s",wifi_ssid); 
    ESP_LOGI(TAG, "wifi_pass:%s",wifi_pass); 

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());  //xTaskCreatePinnedToCore
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_LOGI(TAG, "strlen(wifi_ssid) + 1:%d",strlen(wifi_ssid) + 1);
    for(uint8_t i=0; i< strlen(wifi_ssid) + 1;i++)
        wifi_config.sta.ssid[i] = wifi_ssid[i];
    ESP_LOGI(TAG, "strlen(wifi_pass) + 1:%d",strlen(wifi_pass) + 1);
    for(uint8_t i=0; i< strlen(wifi_pass) + 1;i++)
        wifi_config.sta.password[i] = wifi_pass[i];
    ESP_LOGI(TAG, "wifi_ssid:%s",wifi_config.sta.ssid); 
    ESP_LOGI(TAG, "wifi_pass:%s",wifi_config.sta.password); 

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    // /* The event will not be processed after unregister */
    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    // vEventGroupDelete(s_wifi_event_group);
}

void wifi_reset(void)
{
    char *wifi_ssid = {0};
    char *wifi_pass = {0};
    wifi_ssid = parameter_read_wifi_ssid();
    wifi_pass = parameter_read_wifi_pass();
    ESP_LOGI(TAG, "wifi_ssid:%s",wifi_ssid); 
    ESP_LOGI(TAG, "wifi_pass:%s",wifi_pass); 

    esp_wifi_stop();
    esp_wifi_disconnect();
    //esp_wifi_deinit();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = BACKUP_EXAMPLE_ESP_WIFI_SSID, //"yyg",
            .password = BACKUP_EXAMPLE_ESP_WIFI_PASS,//"123456789",
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_LOGI(TAG, "strlen(wifi_ssid) + 1:%d",strlen(wifi_ssid) + 1);
    for(uint8_t i=0; i< strlen(wifi_ssid) + 1;i++)
        wifi_config.sta.ssid[i] = wifi_ssid[i];
    ESP_LOGI(TAG, "strlen(wifi_pass) + 1:%d",strlen(wifi_pass) + 1);
    for(uint8_t i=0; i< strlen(wifi_pass) + 1;i++)
        wifi_config.sta.password[i] = wifi_pass[i];
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

// void wifi_inital_set(void)
// {
//     strcpy(connection_para.wifi_ssid,BACKUP_EXAMPLE_ESP_WIFI_SSID);
//     strcpy(connection_para.wifi_pass,BACKUP_EXAMPLE_ESP_WIFI_PASS);
//     strcpy(connection_para.broker_url,BACKUP_MQTT_BROKER_URL);
//     strcpy(connection_para.update_url,CONFIG_EXAMPLE_FIRMWARE_UPG_URL);
//     if(flash_write_parameter() == -1)
//         ESP_LOGI(TAG, "flash_write_parameter_error!");

//     // esp_wifi_stop();
//     // esp_wifi_deinit();
//     // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

//     // wifi_config_t wifi_config = {
//     //     .sta = {
//     //         .ssid = BACKUP_EXAMPLE_ESP_WIFI_SSID, //"yyg",
//     //         .password = BACKUP_EXAMPLE_ESP_WIFI_PASS,//"123456789",
//     //         /* Setting a password implies station will connect to all security modes including WEP/WPA.
//     //          * However these modes are deprecated and not advisable to be used. Incase your Access point
//     //          * doesn't support WPA2, these mode can be enabled by commenting below line */
// 	//      .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//     //     },
//     // };

//     // ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
//     // ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
//     // ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
//     // ESP_ERROR_CHECK( esp_wifi_start() );
// }

void wifi_connect(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}


// int8_t get_rssi(void)
// {
//     uint16_t number = 1;
//     uint16_t ap_count = 0;

//     wifi_ap_record_t ap_info[1];
//     wifi_config_t wifi_sta_cfg;

//     if(s_retry_num>0)
//         return 0;
//     ESP_LOGI(TAG,"start scan");
//     memset(ap_info, 0, sizeof(ap_info));
//     if (esp_wifi_get_config(WIFI_IF_STA, &wifi_sta_cfg) != ESP_OK)//获取已连接的ap参数
//     {
//         ESP_LOGI(TAG, "esp_wifi_get_config err");  
//         return -90;
//     }

//     wifi_scan_config_t scan_config = { 0 };
//     scan_config.ssid = wifi_sta_cfg.sta.ssid;//限制扫描的ap的ssid
//     scan_config.bssid = wifi_sta_cfg.sta.bssid;//限制扫描的ap的mac地址
//     esp_wifi_scan_start(&scan_config, true);//阻塞扫描ap，scan_config为扫描的参数
//     //vTaskDelay(1000 / portTICK_RATE_MS);
//     ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));//获取扫描到的ap信息
//     ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
//     //获取扫描到的ap数量，因为限制了ssid和mac，因此最多只会扫描到1个
//     for (int i = 0; (i < 1) && (i < ap_count); i++) {
//         ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
//         ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
//         ESP_LOGI(TAG, "Channel \t\t%d", ap_info[i].primary);
//         ESP_LOGI(TAG, "BSSID: \t\t%02x:%02x:%02x:%02x:%02x:%02x", 
//                                                         ap_info[i].bssid[0],
//                                                         ap_info[i].bssid[1],
//                                                         ap_info[i].bssid[2],
//                                                         ap_info[i].bssid[3],
//                                                         ap_info[i].bssid[4],
//                                                         ap_info[i].bssid[5]);
//     }
//     esp_wifi_scan_stop(); 
//     //from start to stop need 3210ms
//     ESP_LOGI(TAG,"stop scan\r\n");

//     //net_rssi=ap_info[0].rssi;

//     return ap_info[0].rssi;
// }

int8_t get_rssi(void)
{
    wifi_ap_record_t ap;
    esp_wifi_sta_get_ap_info(&ap);
    //printf("%d\n", ap.rssi);
    ESP_LOGI(TAG, "RSSI \t\t%d", ap.rssi);
    return ap.rssi;
}
void wifi_scan(void)
{
    int8_t value = 0;
    for(;;)
    {
        if(parameter_read_wifi_connection()){
            value = get_rssi();
            parameter_write_rssi(value); 
        }
        //vTaskDelay(600000 / portTICK_RATE_MS);      
        vTaskDelay(6000 / portTICK_RATE_MS);    
    }
}