#ifndef __JPEGD2_H
#define __JPEGD2_H 

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "cdjpeg.h" 
// #include <sys.h> 
#include <setjmp.h>

#ifdef __cplusplus 
extern "C" {
#endif

typedef bool (*lcd_write_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
void mjpegdraw(uint8_t *mjpegbuffer, uint32_t size, uint8_t *outbuffer, lcd_write_cb lcd_cb);
void mjpegdraw_icon(uint8_t *mjpegbuffer, uint32_t size, uint8_t *outbuffer, lcd_write_cb lcd_cb,uint32_t x0,uint32_t y0,uint32_t i_width,uint32_t i_hight);
#ifdef __cplusplus 
}
#endif

#endif
