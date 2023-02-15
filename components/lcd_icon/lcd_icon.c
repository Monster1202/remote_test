
#include "lcd_icon.h"
#include "jpegd2.h"
#include "esp_log.h"
#include "usb_camera.h"
#include "para_list.h"
#include "esp_spiffs.h"
#include "bat_adc.h"
#include "bsp_lcd.h"
#include "rm68120.h"

#define icon1_x 663
#define icon1_y 25
#define icon2_x 743
#define icon2_y 135
#define icon3_y 205
#define icon_size1 34
#define icon_size2 18
#define icon_size3 38

#define ICON_SIZE (8 * 1024)
static const char *TAG = "lcd_icon";
size_t total = 0, used = 0;
void jpg_icon_draw(void *jpg_b,void *lcd_b,size_t i,uint32_t x_0,uint32_t y_0,uint32_t width,uint32_t hight)
{
    //size_t i = 100;
    char file_name[64] = {0};
    sprintf(file_name, "/spiffs/r%03d.jpg", i);
    FILE *fd = fopen(file_name, "r");
    int read_bytes = fread(jpg_b, 1, ICON_SIZE, fd);
    fclose(fd);
    mjpegdraw_icon(jpg_b, read_bytes, lcd_b, lcd_write_bitmap,x_0,y_0,width,hight); //x0 y0 hight width
    //free(file_name);
    ESP_LOGI(TAG, "file_name: %s, fd: %p, read_bytes: %d, free_heap: %d", file_name, fd, read_bytes, esp_get_free_heap_size());
}

void bat_adc_get(void)
{
    uint16_t battery = 0;
    battery = adc_get_voltage()*2;
    parameter_write_battery(battery);
    printf("bat=%d\r\n",battery);
}
int cnt = 0;
void lcd_clear_task(void)
{
    while(1){
        if(cnt % 2000 == 1895){       //2999     
            //esp_restart();
            ESP_LOGI(TAG, "lcd_clear_icon_area1");
            //bsp_lcd_reset();
            lcd_clear_icon_area(lcd_panel, COLOR_BLACK);
            //vTaskDelay(1000 / portTICK_RATE_MS);
            //lcd_clear(lcd_panel, COLOR_BLACK);
            //ESP_LOGI(TAG, "lcd_clear_icon_area2");
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

}
void lcd_icon_task(void)
{
    // parameter_read_centralizer();
    // parameter_read_rotation();
    // parameter_read_nozzle();
    // parameter_read_water();
    // parameter_read_pressure_alarm();
    PARAMETER_REMOTE remote_buf = {0};
    PARAMETER_REMOTE former_state = {0};
    spiff_init();
    uint8_t *jpg_buffer = heap_caps_malloc(ICON_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(jpg_buffer != NULL);
    uint8_t *lcd_buffer = (uint8_t *)heap_caps_malloc(ICON_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(lcd_buffer != NULL);
    adc_init();
    
    int refresh = 1;
    while(1){
        get_remote_parameter(&remote_buf);
        if(remote_buf.wifi_connection != former_state.wifi_connection || refresh){
            if(remote_buf.wifi_connection!=0){
                jpg_icon_draw(jpg_buffer,lcd_buffer,102,icon1_x,icon1_y,icon_size1,icon_size1);  //wifi
            }
            else{
                jpg_icon_draw(jpg_buffer,lcd_buffer,100,icon1_x,icon1_y,icon_size1,icon_size1);  //wifi
            }
        }
        if(remote_buf.battery != former_state.battery || refresh){  // 
            if(remote_buf.battery>=3800){
                jpg_icon_draw(jpg_buffer,lcd_buffer,108,icon2_x,icon1_y,icon_size1,icon_size1);  //bat 
            }
            else if(remote_buf.battery>=3600){
                jpg_icon_draw(jpg_buffer,lcd_buffer,106,icon2_x,icon1_y,icon_size1,icon_size1);  //bat
            }
            else{   // if(remote_buf.battery<=1500)
                jpg_icon_draw(jpg_buffer,lcd_buffer,104,icon2_x,icon1_y,icon_size1,icon_size1);  //bat
            }
        }
        if(remote_buf.nozzle != former_state.nozzle || refresh){
            if(remote_buf.nozzle == 2){
                jpg_icon_draw(jpg_buffer,lcd_buffer,112,icon1_x,icon2_y,icon_size1,icon_size1);  //nozzle-bubble
                jpg_icon_draw(jpg_buffer,lcd_buffer,114,icon2_x,icon2_y,icon_size1,icon_size1);  //nozzle-water 0 
            }
            else if(remote_buf.nozzle == 1){
                jpg_icon_draw(jpg_buffer,lcd_buffer,110,icon1_x,icon2_y,icon_size1,icon_size1);  //nozzle-bubble 0
                jpg_icon_draw(jpg_buffer,lcd_buffer,116,icon2_x,icon2_y,icon_size1,icon_size1);  //nozzle-water
            }
            else{   // if(remote_buf.nozzle == 0)
                jpg_icon_draw(jpg_buffer,lcd_buffer,110,icon1_x,icon2_y,icon_size1,icon_size1);  //nozzle-bubble 0
                jpg_icon_draw(jpg_buffer,lcd_buffer,114,icon2_x,icon2_y,icon_size1,icon_size1);  //nozzle-water 0 
            }
        }
        if(remote_buf.centralizer != former_state.centralizer || refresh){
            if(remote_buf.centralizer == 2){
                jpg_icon_draw(jpg_buffer,lcd_buffer,124,icon1_x,icon3_y,icon_size1,icon_size1); //centralizer 
            }
            else if(remote_buf.centralizer == 1){
                jpg_icon_draw(jpg_buffer,lcd_buffer,122,icon1_x,icon3_y,icon_size1,icon_size1); //centralizer
            }
            else{    // if(remote_buf.centralizer == 0)
                jpg_icon_draw(jpg_buffer,lcd_buffer,120,icon1_x,icon3_y,icon_size1,icon_size1); //centralizer
            }
        }
        if(remote_buf.rotation != former_state.rotation || refresh){
            if(remote_buf.rotation == 2){
                jpg_icon_draw(jpg_buffer,lcd_buffer,134,icon2_x,icon3_y,icon_size1,icon_size1); //rotation  
            }
            else if(remote_buf.rotation == 1){
                jpg_icon_draw(jpg_buffer,lcd_buffer,132,icon2_x,icon3_y,icon_size1,icon_size1); //rotation  
            }
            else{    // if(remote_buf.rotation == 0)
                jpg_icon_draw(jpg_buffer,lcd_buffer,130,icon2_x,icon3_y,icon_size1,icon_size1); //rotation  
            }
        }
        if(remote_buf.water != former_state.water || refresh){
            if(remote_buf.water == 1){
                jpg_icon_draw(jpg_buffer,lcd_buffer,144,icon2_x,380,icon_size2,icon_size2); //senser-water 
            }
            else{  // if(remote_buf.water == 0)
                jpg_icon_draw(jpg_buffer,lcd_buffer,142,icon2_x,380,icon_size2,icon_size2); //senser-water   
            }
        }
        if(remote_buf.pressure_alarm != former_state.pressure_alarm || refresh){
            if(remote_buf.pressure_alarm == 0){
                jpg_icon_draw(jpg_buffer,lcd_buffer,144,icon2_x,430,icon_size2,icon_size2); //senser-pressure
            }  
            else{    // if(remote_buf.pressure_alarm == 1)
                jpg_icon_draw(jpg_buffer,lcd_buffer,142,icon2_x,430,icon_size2,icon_size2); //senser-pressure 0  
            }
        }

        // if(remote_buf.sta_brush != former_state.sta_brush || refresh){
        //     if(remote_buf.sta_brush == 2){
        //         jpg_icon_draw(jpg_buffer,lcd_buffer,142,icon2_x,430,18,18); //status-brush  
        //     }
        //     else if(remote_buf.sta_brush == 1){
        //         jpg_icon_draw(jpg_buffer,lcd_buffer,144,icon2_x,430,18,18); //status-brush     
        //     }
        //     else{      // if(remote_buf.sta_brush == 0)
        //         jpg_icon_draw(jpg_buffer,lcd_buffer,140,icon2_x,430,18,18); //status-brush     
        //     }
        // }
        get_remote_parameter(&former_state);
        vTaskDelay(100 / portTICK_RATE_MS);
        cnt++;
        refresh = 0;
        if(cnt % 300 == 1){
            bat_adc_get();
        }  
        // else if(cnt % 200 == 22){       //2999     
        //     refresh = 1;
        //     //esp_restart();
        //     //ESP_LOGI(TAG, "lcd_clear_icon_area1");
        //     //bsp_lcd_reset();
        //     //lcd_clear_icon_area(lcd_panel, COLOR_BLACK);
        //     //vTaskDelay(1000 / portTICK_RATE_MS);
        //     //lcd_clear(lcd_panel, COLOR_BLACK);
        //     //ESP_LOGI(TAG, "lcd_clear_icon_area2");
        // }
        else if(cnt % 2000 == 1900){    
            refresh = 1;
        }
        //parameter_write_refresh(refresh);
    }
    free(lcd_buffer);
    free(jpg_buffer);
}
void spiff_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    // size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
        //ESP_LOGI(TAG, "Partition table: %s", conf.partition_label);
    }
    // All done, unmount partition and disable SPIFFS
    // esp_vfs_spiffs_unregister(conf.partition_label);
    // ESP_LOGI(TAG, "SPIFFS unmounted");
}



