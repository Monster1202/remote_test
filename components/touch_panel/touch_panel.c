
#include "touch_panel.h"
#include "ft5x06.h"
#include "para_list.h"
#include "mqtt_app.h"
#include "gpio_ctrl.h"

#define touch_width 30
#define button1_x 680
#define button1_y 155
#define button2_x 760
#define button3_y 225
#define button5_y 295
static const char *TAG = "touch_panel";

void touch_input(void)
{
    uint8_t register_value = 0;
    uint8_t register_afterpress = 0;
    int16_t buf_x = 0;
    int16_t buf_y = 0;
    //uint8_t state = 0;
    touchpad_init();
    touch_t touchpanel;
    printf("touchpad_init");
    vTaskDelay(50 / portTICK_RATE_MS);
    int cnt = 0;
    while(1){
        vTaskDelay(50 / portTICK_RATE_MS);
        // printf("touchpad_read");
        // vTaskDelay(4000 / portTICK_RATE_MS);
        touchpad_read(&touchpanel);
        if(touchpanel.state){
            printf("touchpanel:%d,%d,%d\r\n",touchpanel.state,touchpanel.x,touchpanel.y);
            if(buf_x == touchpanel.x && buf_y == touchpanel.y)
            {
                //long press
            }
            else {
                if(touchpanel.x>=(button1_x-touch_width) && touchpanel.x<=(button1_x+touch_width) && touchpanel.y>=(button1_y-touch_width) && touchpanel.y<=(button1_y+touch_width))  //touch button1
                {
                    register_value = parameter_read_nozzle();
                    register_afterpress = UI_press_output(register_value,2);
                    nozzle_io_out(register_afterpress);
                    device_states_publish(3); 
                    printf("touch button1\r\n"); 
                }
                else if(touchpanel.x>=(button2_x-touch_width) && touchpanel.x<=(button2_x+touch_width) && touchpanel.y>=(button1_y-touch_width) && touchpanel.y<=(button1_y+touch_width))  //touch button2
                {
                    register_value = parameter_read_nozzle();
                    register_afterpress = UI_press_output(register_value,1);
                    nozzle_io_out(register_afterpress);
                    device_states_publish(3); 
                    printf("touch button2\r\n"); 
                }
                else if(touchpanel.x>=(button1_x-touch_width) && touchpanel.x<=(button1_x+touch_width) && touchpanel.y>=(button3_y-touch_width) && touchpanel.y<=(button3_y+touch_width))  //touch button3
                {
                    register_value = parameter_read_centralizer();
                    register_afterpress = UI_press_output(register_value,1);
                    centralizer_io_out(register_afterpress);
                    device_states_publish(1);   
                    printf("touch button3\r\n"); 
                }
                else if(touchpanel.x>=(button2_x-touch_width) && touchpanel.x<=(button2_x+touch_width) && touchpanel.y>=(button3_y-touch_width) && touchpanel.y<=(button3_y+touch_width))  //touch button4
                {
                    register_value = parameter_read_rotation();
                    register_afterpress = UI_press_output(register_value,1);
                    rotation_io_out(register_afterpress);
                    device_states_publish(2); 
                    printf("touch button4\r\n"); 
                }
                else if(touchpanel.x>=(button1_x-touch_width) && touchpanel.x<=(button1_x+touch_width) && touchpanel.y>=(button5_y-touch_width) && touchpanel.y<=(button5_y+touch_width))  //touch button5
                {
                    register_value = parameter_read_centralizer();
                    register_afterpress = UI_press_output(register_value,2);
                    centralizer_io_out(register_afterpress);
                    device_states_publish(1);     
                    printf("touch button5\r\n"); 
                }
                else if(touchpanel.x>=(button2_x-touch_width) && touchpanel.x<=(button2_x+touch_width) && touchpanel.y>=(button5_y-touch_width) && touchpanel.y<=(button5_y+touch_width))  //touch button6
                {
                    register_value = parameter_read_rotation();
                    register_afterpress = UI_press_output(register_value,2);
                    rotation_io_out(register_afterpress);
                    device_states_publish(2); 
                    printf("touch button6\r\n"); 
                }
            }
            buf_x = touchpanel.x;
            buf_y = touchpanel.y;
        }
        //cnt++;
        if(touchpanel.state ==1 && touchpanel.x == 4095 && touchpanel.y == -3615)
            vTaskDelay(5000 / portTICK_RATE_MS);
    }
}
/*Initialize your touchpad*/
static void touchpad_init(void)
{
    ft5x06_init();
}

/*Will be called by the library to read the touchpad*/
uint8_t touchpad_read(touch_t *data)
{
    static int16_t last_x = 0;
    static int16_t last_y = 0;
    
    /*Save the pressed coordinates and the state*/
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = 1;
    } else {
        data->state = 0;
    }
    /*Set the last pressed coordinates*/
    data->x = last_x;
    data->y = last_y;
    //printf("data->state:%d",data->state);
    //printf("touchpanel:%d,%d,%d\r\n",data->state,last_x,last_y);
    return data->state;
}

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    static uint8_t touch_points_num = 0;
    ft5x06_read_byte(FT5x06_TOUCH_POINTS, &touch_points_num);
    if(touch_points_num)    return true;
    else                    return false;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(int16_t * x, int16_t * y)
{
    static uint8_t data[4];
    ft5x06_read_bytes(FT5x06_TOUCH1_XH, 4, data);
    // *x = ((data[0] & 0x0f) << 8) + data[1];
    // *y = ((data[2] & 0x0f) << 8) + data[3];
    *y = 480 - ((data[0] & 0x0f) << 8) - data[1];
    *x = ((data[2] & 0x0f) << 8) + data[3];
}
