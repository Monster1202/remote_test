#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "gpio_ctrl.h"

#include "mqtt_app.h"
#include "wifi_sta.h"
#include "tca9554.h"

#define GPIO_INPUT_IO_1     35
#define GPIO_INPUT_IO_2     36
#define GPIO_INPUT_IO_3     37
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_1)|(1ULL<<GPIO_INPUT_IO_2)|(1ULL<<GPIO_INPUT_IO_3))
// #define GPIO_OUTPUT_IO_1     35
// #define GPIO_OUTPUT_IO_2     36
// #define GPIO_OUTPUT_IO_3     37
//#define GPIO_OUTPUT_PIN_SEL  ( (1ULL<<GPIO_LED_RUN)| (1ULL<<GPIO_LED_ERR))
#define ESP_INTR_FLAG_DEFAULT 0
#define KEY_SPEED_LONG 200 //long press debug time(ms)
#define KEY_SPEED_DOUBLE 10 //double press debug time(ms)
#define KEY_ONCE 1
#define KEY_TWICE 2
#define KEY_LONG 3

uint8_t flag_mqtt_test = 0;
static const char *TAG = "GPIO_CTRL";
//gpio
static xQueueHandle gpio_evt_queue = NULL;
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    uint8_t buf_state = 0;
    int time_cnt = 0;
    int cnt = 0;
    uint8_t wifi_sta,s_led_state = 0;
    for(;;) {
        // if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
        //     buf_state = gpio_get_level(io_num);
        //     ESP_LOGI(TAG,"GPIO[%d] intr, val: %d", io_num, buf_state);
        //     //if(buf_state ==1)
        //     sw_key_read(io_num,buf_state); //judge button press once twice or long
        // }
        cnt++;
        if(cnt % 20 == 1){
            wifi_sta=parameter_read_wifi_connection();
            time_cnt++;
            if(wifi_sta>=1){
                if(time_cnt % (5-wifi_sta) == 1){
                    s_led_state = !s_led_state;
                    gpio_set_level(GPIO_LED_RUN, s_led_state);
                    //gpio_set_level(GPIO_LED_ERR, s_led_state);
                }
                if(wifi_sta!=3)
                    gpio_set_level(GPIO_LED_ERR, 0);
                else
                    gpio_set_level(GPIO_LED_ERR, 1);
            }
        }
        tca9554_ext_io_read();
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}


uint8_t UI_press_output(uint8_t value,uint8_t button)
{
    if(value==button)
        value = 0;
    else
        value = button;
    return value;
}

#ifdef DEVICE_TYPE_REMOTE
void centralizer_io_out(uint8_t value)
{
    if(value == 1){
        // gpio_set_level(GPIO_OUTPUT_LED_1, 1);
        // gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        parameter_write_centralizer(1);
        //ESP_LOGI(TAG, "remote_para.centralizer = 1");
        }
    else if(value == 2){
        // gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_2, 1);
        parameter_write_centralizer(2);
        //ESP_LOGI(TAG, "remote_para.centralizer = 2");
        }
    else{
        // gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        parameter_write_centralizer(0);
        //ESP_LOGI(TAG, "remote_para.centralizer = 0");
        }
}
void rotation_io_out(uint8_t value)
{
    if(value==1){
        // gpio_set_level(GPIO_OUTPUT_LED_3, 1);
        // gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        parameter_write_rotation(1);
        //ESP_LOGI(TAG, "remote_para.rotation = 1");
        }
    else if(value==2){
        // gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_4, 1);
        parameter_write_rotation(2);
        //ESP_LOGI(TAG, "remote_para.rotation = 2");
        }
    else{
        // gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        parameter_write_rotation(0);
        //ESP_LOGI(TAG, "remote_para.rotation = 0");
        } 
}
void nozzle_io_out(uint8_t value)
{
    if(value == 1){
        // gpio_set_level(GPIO_OUTPUT_LED_5, 1);
        // gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        parameter_write_nozzle(1);
        //ESP_LOGI(TAG, "remote_para.nozzle = 1");
        }
    else if(value == 2){
        // gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_6, 1);
        parameter_write_nozzle(2);
        //ESP_LOGI(TAG, "remote_para.nozzle = 2");
        }
    else{
        // gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        parameter_write_nozzle(0);
        //ESP_LOGI(TAG, "remote_para.nozzle = 0");
        }
}

void remote_stop_io_out(uint8_t value , uint8_t state) //state 0 from press dom't change parameter
{
    if(value == 1){
        // gpio_set_level(GPIO_OUTPUT_LED_1, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_2, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_3, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_4, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_5, 0);
        // gpio_set_level(GPIO_OUTPUT_LED_6, 0);
        parameter_write_heater(0);
        parameter_write_mode(0);
        parameter_write_centralizer(0);
        parameter_write_rotation(0);
        parameter_write_nozzle(0);
        if(state){
            parameter_write_emergency_stop(1);
            //gpio_set_level(GPIO_SYS_LED, 1);
            //ESP_LOGI(TAG, "remote_para.emergency_stop = 1");
            }
            }
    else{
        if(state){
            parameter_write_emergency_stop(0);
            //gpio_set_level(GPIO_SYS_LED, 0);
            //ESP_LOGI(TAG, "remote_para.emergency_stop = 0");
            }
            }
}

// void remote_press_output(uint8_t io_num)
// {
//     uint8_t register_value = 0;
//     uint8_t register_afterpress = 0;
//     uint8_t register_emergency_stop = 0;
//     register_emergency_stop = parameter_read_emergency_stop();
//     uint8_t mqtt_connected = 0;
//     mqtt_connected = parameter_read_wifi_connection();
//     // if(register_emergency_stop)
//     // {
//     //     // if(io_num == GPIO_INPUT_IO_STOP)
//     //     // {
//     //     //     ESP_LOGI(TAG, "back to normal mode");
//     //     //     register_value = parameter_read_emergency_stop();
//     //     //     register_afterpress = UI_press_output(register_value,1);
//     //     //     remote_stop_io_out(register_afterpress,0);
//     //     //     device_states_publish(4);
//     //     // }
//     //     // else{
//     //     ESP_LOGI(TAG, "emergency_stop_error_press");
//     //     //timer_periodic();
//     //     //}
//     // }
//     // else
//     // {
//         switch(io_num)   //BLISTER
//         {
//             case GPIO_INPUT_IO_1:
//             ESP_LOGI(TAG, "GPIO_INPUT_IO_1");
//             register_value = parameter_read_centralizer();
//             register_afterpress = UI_press_output(register_value,1);
//             centralizer_io_out(register_afterpress);
//             device_states_publish(1);            
//             break;
//             case GPIO_INPUT_IO_2:
//             ESP_LOGI(TAG, "GPIO_INPUT_IO_2");
//             register_value = parameter_read_centralizer();
//             register_afterpress = UI_press_output(register_value,2);
//             centralizer_io_out(register_afterpress);
//             device_states_publish(1); 
//             break;
//             case GPIO_INPUT_IO_3:
//             ESP_LOGI(TAG, "GPIO_INPUT_IO_3");
//             register_value = parameter_read_rotation();
//             register_afterpress = UI_press_output(register_value,1);
//             rotation_io_out(register_afterpress);
//             device_states_publish(2); 
//             break;
//             case GPIO_INPUT_IO_4:
//             ESP_LOGI(TAG, "GPIO_INPUT_IO_4");
//             register_value = parameter_read_rotation();
//             register_afterpress = UI_press_output(register_value,2);
//             rotation_io_out(register_afterpress);
//             device_states_publish(2); 
//             break;
//             case GPIO_INPUT_IO_5:
//             ESP_LOGI(TAG, "GPIO_INPUT_IO_5");
//             register_value = parameter_read_nozzle();
//             register_afterpress = UI_press_output(register_value,1);
//             nozzle_io_out(register_afterpress);
//             device_states_publish(3); 
//             break;
//             case GPIO_INPUT_IO_6:
//             ESP_LOGI(TAG, "GPIO_INPUT_IO_6");
//             register_value = parameter_read_nozzle();
//             register_afterpress = UI_press_output(register_value,2);
//             nozzle_io_out(register_afterpress);
//             device_states_publish(3); 
//             break;
//             // case GPIO_INPUT_IO_7:
//             // ESP_LOGI(TAG, "GPIO_INPUT_IO_7");
//             // break;
//             // case GPIO_INPUT_IO_STOP:
//             // ESP_LOGI(TAG, "GPIO_INPUT_IO_STOP");
//             // register_value = parameter_read_emergency_stop();
//             // register_afterpress = UI_press_output(register_value,1);
//             // remote_stop_io_out(register_afterpress,0);
//             // device_states_publish(4);
//             // break;
//             default:
//             //ESP_LOGI(TAG, "KEY_default");
//             break;
//         }
//     //}
// }
#endif



uint8_t KEY_READ(uint8_t io_num)
{
    uint16_t b,c;
    if(!gpio_get_level(io_num))  //button press  gpio_get_level(io_num)
    {
        c = 0;
        vTaskDelay(20 / portTICK_RATE_MS);//delay_us(20000);  //delay debounce
        if(!gpio_get_level(io_num))  //check again
        {
            while((!gpio_get_level(io_num)) && c<KEY_SPEED_LONG) //long press time counting
            {
                c++;
                vTaskDelay(10 / portTICK_RATE_MS);//delay_us(10000); //10ms
            }
            if(c>=KEY_SPEED_LONG)
            {
                while(!gpio_get_level(io_num)){}
                return KEY_LONG;                   
            }
            else{   
                for(b=0;b<KEY_SPEED_DOUBLE;b++)  //double press check
                {
                    vTaskDelay(20 / portTICK_RATE_MS);//delay_us(20000);
                    if(!gpio_get_level(io_num))
                    {
                        while(!gpio_get_level(io_num)){}
                        return KEY_TWICE;
                    }
                }
            }
            return KEY_ONCE; //single press
        }
    }
    return 0; //no press
}
void sw_key_read(uint8_t io_num,uint8_t state)
{
    uint8_t key_status = 0;
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    // if(state == 1)
    // {
    // #ifdef DEVICE_TYPE_REMOTE
    // remote_press_output(io_num);
    // #endif
    // }
    // if(io_num == GPIO_INPUT_IO_STOP)
    // {
        key_status=KEY_READ(io_num);
        //ESP_LOGI(TAG, "GPIO[%d] intr, val: %d", io_num, gpio_get_level(io_num));
        switch(key_status)
        {
            case KEY_ONCE:
            ESP_LOGI(TAG, "KEY_ONCE");
            switch(io_num)   //BLISTER
            {
                case GPIO_INPUT_IO_1:
                ESP_LOGI(TAG, "GPIO_INPUT_IO_1");
                register_value = parameter_read_centralizer();
                register_afterpress = UI_press_output(register_value,1);
                centralizer_io_out(register_afterpress);
                device_states_publish(1);            
                break;
                case GPIO_INPUT_IO_2:
                ESP_LOGI(TAG, "GPIO_INPUT_IO_2");
                register_value = parameter_read_rotation();
                register_afterpress = UI_press_output(register_value,1);
                rotation_io_out(register_afterpress);
                device_states_publish(2); 
                break;
                case GPIO_INPUT_IO_3:
                ESP_LOGI(TAG, "GPIO_INPUT_IO_3");
                register_value = parameter_read_nozzle();
                register_afterpress = UI_press_output(register_value,1);
                nozzle_io_out(register_afterpress);
                device_states_publish(3); 
                break;
            }
            break;
            case KEY_TWICE:
            ESP_LOGI(TAG, "KEY_TWICE");
            switch(io_num)   //BLISTER
            {
                case GPIO_INPUT_IO_1:
                ESP_LOGI(TAG, "GPIO_INPUT_IO_1");
                register_value = parameter_read_centralizer();
                register_afterpress = UI_press_output(register_value,2);
                centralizer_io_out(register_afterpress);
                device_states_publish(1);       
                break;
                case GPIO_INPUT_IO_2:
                ESP_LOGI(TAG, "GPIO_INPUT_IO_2");
                register_value = parameter_read_rotation();
                register_afterpress = UI_press_output(register_value,2);
                rotation_io_out(register_afterpress);
                device_states_publish(2); 
                break;
                case GPIO_INPUT_IO_3:
                ESP_LOGI(TAG, "GPIO_INPUT_IO_3");
                register_value = parameter_read_nozzle();
                register_afterpress = UI_press_output(register_value,2);
                nozzle_io_out(register_afterpress);
                device_states_publish(3); 
                break;
            }
            break;
            case KEY_LONG:
            ESP_LOGI(TAG, "KEY_LONG");  
            // wifi_url_inital_set_para();
            // vTaskDelay(1000 / portTICK_RATE_MS);
            // wifi_reset();
            // vTaskDelay(1000 / portTICK_RATE_MS);
            // mqtt_reset();         
            // #ifdef mqtt_test
            //     register_afterpress = UI_press_output(flag_mqtt_test,1);
            //     flag_mqtt_test = register_afterpress;
            // #endif 
            break;
            default:
            //ESP_LOGI(TAG, "KEY_default");
            break;
        }
    //}

    vTaskDelay(10 / portTICK_RATE_MS);
}


void gpio_init(void)
{
    //GPIO
    //esp_log_level_set("GPIO_CTRL", ESP_LOG_DEBUG);  //ESP_LOG_DEBUG ESP_LOG_INFO ESP_LOG_WARN
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_set_level(GPIO_LED_RUN, 0);
    //gpio_set_level(GPIO_LED_ERR, 0);
    //     //interrupt of rising edge
    // io_conf.intr_type = GPIO_INTR_ANYEDGE;//GPIO_INTR_POSEDGE;
    // //bit mask of the pins, use GPIO4/5 here
    // io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // //set as input mode
    // io_conf.mode = GPIO_MODE_INPUT;
    // //enable pull-up mode
    // io_conf.pull_up_en = 1;
    // gpio_config(&io_conf);

    // //change gpio intrrupt type for one pin
    // //gpio_set_intr_type(GPIO_INPUT_IO_STOP, GPIO_INTR_ANYEDGE);

    // //create a queue to handle gpio event from isr
    // gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreatePinnedToCore(gpio_task_example, "gpio_task_example", 4096, NULL, 10, NULL,0);
    //xTaskCreatePinnedToCore(lcd_icon_task, "lcd_icon_task", 8192, NULL, 24, NULL,  0);
    
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    //gpio_isr_handler_add(GPIO_INPUT_IO_STOP, gpio_isr_handler, (void*) GPIO_INPUT_IO_STOP);
    //hook isr handler for specific gpio pin

    // gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);
    // gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, (void*) GPIO_INPUT_IO_2);
    // gpio_isr_handler_add(GPIO_INPUT_IO_3, gpio_isr_handler, (void*) GPIO_INPUT_IO_3);

    // gpio_isr_handler_add(GPIO_INPUT_IO_4, gpio_isr_handler, (void*) GPIO_INPUT_IO_4);
    // gpio_isr_handler_add(GPIO_INPUT_IO_5, gpio_isr_handler, (void*) GPIO_INPUT_IO_5);
    // gpio_isr_handler_add(GPIO_INPUT_IO_6, gpio_isr_handler, (void*) GPIO_INPUT_IO_6);
    // gpio_isr_handler_add(GPIO_INPUT_IO_7, gpio_isr_handler, (void*) GPIO_INPUT_IO_7);
    //remove isr handler for gpio number.
    //gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin again
    //gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    ESP_LOGI(TAG, "Minimum free heap size: %d bytes", esp_get_minimum_free_heap_size());
}

uint8_t tca9554_input_get(void)
{
    uint8_t pin_val = 0;
    ESP_ERROR_CHECK(tca9554_read_input_pins(&pin_val));
    pin_val = pin_val&0x7F;
    return pin_val;
}
uint8_t PLUS_KEY_READ(uint8_t value)
{
    uint16_t b,c;
    // uint8_t pin_val = 0;
    // uint8_t io_num;
    //if(tca9554_read_input_pins(&pin_val) == value){}
    if(tca9554_input_get() == value)  //button press  gpio_get_level(io_num)
    {
        c = 0;
        vTaskDelay(20 / portTICK_RATE_MS);//delay_us(20000);  //delay debounce
        if(tca9554_input_get() == value)  //check again
        {
            while((tca9554_input_get() == value) && c<KEY_SPEED_LONG) //long press time counting
            {
                c++;
                vTaskDelay(10 / portTICK_RATE_MS);//delay_us(10000); //10ms
            }
            if(c>=KEY_SPEED_LONG)
            {
                while(tca9554_input_get() == value){}
                return KEY_LONG;                   
            }
            else{   
                for(b=0;b<KEY_SPEED_DOUBLE;b++)  //double press check
                {
                    vTaskDelay(20 / portTICK_RATE_MS);//delay_us(20000);
                    if(tca9554_input_get() == value)
                    {
                        while(tca9554_input_get() == value){}
                        return KEY_TWICE;
                    }
                }
            }
            return KEY_ONCE; //single press
        }
    }
    return 0; //no press
}


uint8_t tca9554_ext_io_read(void)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    uint8_t input_val = 0;
    uint8_t input_cnt = 0;
    input_val = tca9554_input_get();
    //printf("P0~P7=0x%02X\r\n", input_val);
    if(input_val != 0xFF){
        switch(input_val)   //BLISTER
        {
            case 0x5F:
            input_cnt = PLUS_KEY_READ(0x5F);
            ESP_LOGI(TAG, "BUTTON1 PRESSED:%d",input_cnt);
            register_value = parameter_read_centralizer();
            register_afterpress = UI_press_output(register_value,1);
            centralizer_io_out(register_afterpress);
            device_states_publish(1);            
            break;
            case 0x6F:
            input_cnt = PLUS_KEY_READ(0x6F);
            ESP_LOGI(TAG, "BUTTON2 PRESSED:%d",input_cnt);
            register_value = parameter_read_centralizer();
            register_afterpress = UI_press_output(register_value,2);
            centralizer_io_out(register_afterpress);
            device_states_publish(1);       
            break;
            case 0x77:
            input_cnt = PLUS_KEY_READ(0x77);
            ESP_LOGI(TAG, "BUTTON3 PRESSED:%d",input_cnt);
            register_value = parameter_read_rotation();
            register_afterpress = UI_press_output(register_value,1);
            rotation_io_out(register_afterpress);
            device_states_publish(2); 
            break;
            case 0x7B:
            input_cnt = PLUS_KEY_READ(0x7B);
            ESP_LOGI(TAG, "BUTTON4 PRESSED:%d",input_cnt);
            register_value = parameter_read_rotation();
            register_afterpress = UI_press_output(register_value,2);
            rotation_io_out(register_afterpress);
            device_states_publish(2); 
            break;
            case 0x7D:
            input_cnt = PLUS_KEY_READ(0x7D);
            ESP_LOGI(TAG, "BUTTON5 PRESSED:%d",input_cnt);
            register_value = parameter_read_nozzle();
            register_afterpress = UI_press_output(register_value,1);
            nozzle_io_out(register_afterpress);
            //device_states_publish(3); 
            device_states_publish(10);
            break;
            case 0x7E:
            input_cnt = PLUS_KEY_READ(0x7E);
            ESP_LOGI(TAG, "BUTTON6 PRESSED:%d",input_cnt);
            register_value = parameter_read_nozzle();
            register_afterpress = UI_press_output(register_value,2);
            nozzle_io_out(register_afterpress);
            //device_states_publish(3); 
            device_states_publish(11);
            break;
            case 0x3F:
            input_cnt = PLUS_KEY_READ(0x3F);
            ESP_LOGI(TAG, "BUTTON7 PRESSED:%d",input_cnt);
            remote_stop_io_out(1,0);
            device_states_publish(4); 
            break;
        }
    }
    return input_val;
}




// #ifdef mqtt_test
// void mqtt_gpio_test(void* arg)
// {
//     int cnt = 0;
//     for(;;) {
//         if(flag_mqtt_test)
//         {
//             //device_states_publish(cnt%4+1);  
//             remote_press_output(cnt%6+35);  
//             ESP_LOGI(TAG,"mqtt_gpio_test_cnt: %d\n", cnt++);
//             //printf("mqtt_gpio_test_cnt: %d\n", cnt++);
//         }
        
//         vTaskDelay(100 / portTICK_RATE_MS);
//     }
// }
// #endif