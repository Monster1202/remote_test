
#pragma once

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

#define WIDTH_PLUS 2
#define HIGHT_PLUS 2

#define CONFIG_LCD_BUF_WIDTH 320
#define CONFIG_LCD_BUF_HIGHT (48/HIGHT_PLUS/2)  //48   8/2 mohu
_Bool lcd_write_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
void print_heapsize(void);
void lcd_icon_task(void);
void spiff_init(void);
/* USB Camera Descriptors Related MACROS,
the quick demo skip the standred get descriptors process,
users need to get params from camera descriptors from PC side,
eg. run `lsusb -v` in linux,
then hardcode the related MACROS below
*/
#if 0  //config 1
#define DESCRIPTOR_CONFIGURATION_INDEX 1          
#define DESCRIPTOR_FORMAT_MJPEG_INDEX  1     //FormatIndex

#define DESCRIPTOR_FRAME_320_240_INDEX 1
#define DESCRIPTOR_FRAME_176_144_INDEX 2
#define DESCRIPTOR_FRAME_160_120_INDEX 3

#define DESCRIPTOR_FRAME_5FPS_INTERVAL  2000000     //FrameInterval
#define DESCRIPTOR_FRAME_10FPS_INTERVAL 1000000
#define DESCRIPTOR_FRAME_15FPS_INTERVAL 666666
#define DESCRIPTOR_FRAME_25FPS_INTERVAL 400000
#define DESCRIPTOR_FRAME_30FPS_INTERVAL 333333

#define DESCRIPTOR_STREAM_INTERFACE_INDEX   1         //bInterfaceNumber  
#define DESCRIPTOR_STREAM_INTERFACE_ALT_MPS_512  4

#define DESCRIPTOR_STREAM_ISOC_ENDPOINT_ADDR 0x82     //EndpointAddress
#else  //config 2
#define DESCRIPTOR_CONFIGURATION_INDEX 1          
#define DESCRIPTOR_FORMAT_MJPEG_INDEX  2     //FormatIndex

#define DESCRIPTOR_FRAME_640_480_INDEX 1     //FrameIndex
#define DESCRIPTOR_FRAME_352_288_INDEX 2
#define DESCRIPTOR_FRAME_320_240_INDEX 3

#define DESCRIPTOR_FRAME_5FPS_INTERVAL  2000000     //FrameInterval
#define DESCRIPTOR_FRAME_10FPS_INTERVAL 1000000
#define DESCRIPTOR_FRAME_15FPS_INTERVAL 666666
#define DESCRIPTOR_FRAME_25FPS_INTERVAL 400000
#define DESCRIPTOR_FRAME_30FPS_INTERVAL 333333

#define DESCRIPTOR_STREAM_INTERFACE_INDEX   1         //bInterfaceNumber  
#define DESCRIPTOR_STREAM_INTERFACE_ALT_MPS_512  4

#define DESCRIPTOR_STREAM_ISOC_ENDPOINT_ADDR 0x82     //EndpointAddress
// #else  //config 3
// #define DESCRIPTOR_CONFIGURATION_INDEX 1          
// #define DESCRIPTOR_FORMAT_MJPEG_INDEX  1     //FormatIndex

// #define DESCRIPTOR_FRAME_120_160_INDEX 3     //FrameIndex
// #define DESCRIPTOR_FRAME_144_176_INDEX 2
// #define DESCRIPTOR_FRAME_320_240_INDEX 1

// #define DESCRIPTOR_FRAME_5FPS_INTERVAL  2000000     //FrameInterval
// #define DESCRIPTOR_FRAME_10FPS_INTERVAL 1000000
// #define DESCRIPTOR_FRAME_15FPS_INTERVAL 666666
// #define DESCRIPTOR_FRAME_25FPS_INTERVAL 400000
// #define DESCRIPTOR_FRAME_30FPS_INTERVAL 333333

// #define DESCRIPTOR_STREAM_INTERFACE_INDEX   1         //bInterfaceNumber  
// #define DESCRIPTOR_STREAM_INTERFACE_ALT_MPS_512  4    //bAlternateSetting

// #define DESCRIPTOR_STREAM_ISOC_ENDPOINT_ADDR 0x82     //EndpointAddress

#endif

/* Demo Related MACROS */
#define DEMO_FRAME_WIDTH   320
#define DEMO_FRAME_HEIGHT  240
#define DEMO_XFER_BUFFER_SIZE (35 * 1024) //Double buffer
#define DEMO_FRAME_INDEX DESCRIPTOR_FRAME_320_240_INDEX
#define DEMO_FRAME_INTERVAL DESCRIPTOR_FRAME_15FPS_INTERVAL

/* max packet size of esp32-s2 is 1*512, bigger is not supported*/
#define DEMO_ISOC_EP_MPS   512
#define DEMO_ISOC_INTERFACE_ALT DESCRIPTOR_STREAM_INTERFACE_ALT_MPS_512

#define DEMO_MAX_TRANFER_SIZE (CONFIG_LCD_BUF_WIDTH * CONFIG_LCD_BUF_HIGHT * 2*WIDTH_PLUS*HIGHT_PLUS + 64)
//#define DEMO_MAX_TRANFER_SIZE (640 * 480*2)

//void lv_example_sjpg_2(void);
#ifdef __cplusplus
}
#endif



