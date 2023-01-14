#ifndef _BSP_EXT_IO_H_
#define _BSP_EXT_IO_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
typedef struct {
    union {
        struct {
            uint8_t ext_io0:1;
            uint8_t ext_io1:1;
            uint8_t ext_io2:1;
            uint8_t ext_io3:1;
            uint8_t lcd_bl:1;
            uint8_t audio_pa:1;
            uint8_t boost_en:1;
            uint8_t tp_rst:1;
        };
        uint8_t val;
    };
} ext_io_t;

#define BSP_EXT_IO_DEFAULT_CONFIG() {   \
        .ext_io0 = 1,                   \
        .ext_io1 = 1,                   \
        .ext_io2 = 1,                   \
        .ext_io3 = 1,                   \
        .lcd_bl = 0,                    \
        .audio_pa = 0,                  \
        .boost_en = 0,                  \
        .tp_rst = 0,                    \
    }

#define BSP_EXT_IO_DEFAULT_LEVEL() {    \
        .ext_io0 = 0,                   \
        .ext_io1 = 0,                   \
        .ext_io2 = 0,                   \
        .ext_io3 = 0,                   \
        .lcd_bl = 1,                    \
        .audio_pa = 1,                  \
        .boost_en = 0,                  \
        .tp_rst = 1,                    \
    }

#define BSP_EXT_IO_SLEEP_LEVEL() {      \
        .ext_io0 = 0,                   \
        .ext_io1 = 0,                   \
        .ext_io2 = 0,                   \
        .ext_io3 = 0,                   \
        .lcd_bl = 0,                    \
        .audio_pa = 0,                  \
        .boost_en = 0,                  \
        .tp_rst = 0,                    \
    }

#define BSP_EXT_IO_OUTPUT_CONFIG() {   \
        .ext_io0 = 0,                   \
        .ext_io1 = 0,                   \
        .ext_io2 = 0,                   \
        .ext_io3 = 0,                   \
        .lcd_bl = 0,                    \
        .audio_pa = 0,                  \
        .boost_en = 0,                  \
        .tp_rst = 0,                    \
    }

#define BSP_EXT_IO_OUTPUT_DEFAULT_LEVEL() {    \
        .ext_io0 = 0,                   \
        .ext_io1 = 0,                   \
        .ext_io2 = 0,                   \
        .ext_io3 = 0,                   \
        .lcd_bl = 1,                    \
        .audio_pa = 1,                  \
        .boost_en = 0,                  \
        .tp_rst = 1,                    \
    }
*/

/*
    typedef struct {
    union {
        struct {
            uint8_t motion_int:1;
            uint8_t ext_io2:1;
            uint8_t ext_io0:1;
            uint8_t ext_io1:1;
            uint8_t lcd_bl:1;
            uint8_t audio_pa:1;
            uint8_t tp_int:1;
            uint8_t tp_rst:1;
        };
        uint8_t val;
    };
} ext_io_t;

#define BSP_EXT_IO_DEFAULT_CONFIG() {   \
        .motion_int = 1,                \
        .ext_io2 = 1,                   \
        .ext_io0 = 1,                   \
        .ext_io1 = 1,                   \
        .lcd_bl = 0,                    \
        .audio_pa = 0,                  \
        .tp_int = 1,                    \
        .tp_rst = 0,                    \
    }

#define BSP_EXT_IO_DEFAULT_LEVEL() {    \
        .motion_int = 1,                \
        .ext_io2 = 1,                   \
        .ext_io0 = 1,                   \
        .ext_io1 = 1,                   \
        .lcd_bl = 1,                    \
        .audio_pa = 1,                  \
        .tp_int = 1,                    \
        .tp_rst = 1,                    \
    }

#define BSP_EXT_IO_SLEEP_LEVEL() {      \
        .motion_int = 1,                \
        .ext_io2 = 1,                   \
        .ext_io0 = 1,                   \
        .ext_io1 = 1,                   \
        .lcd_bl = 0,                    \
        .audio_pa = 0,                  \
        .tp_int = 1,                    \
        .tp_rst = 0,                    \
    }
    */

typedef struct {
    union {
        struct {
            uint8_t bubble:1;
            uint8_t water:1;
            uint8_t stretch:1;
            uint8_t draw:1;
            uint8_t rotate_ck:1;
            uint8_t rotate_an:1;
            uint8_t emergency:1;
            uint8_t back_xx:1;
        };
        uint8_t val;
    };
} ext_io_t;

//output 0  input 1
#define BSP_EXT_IO_DEFAULT_CONFIG() {   \
        .bubble = 1,                \
        .water = 1,                   \
        .stretch = 1,                   \
        .draw = 1,                   \
        .rotate_ck = 1,                    \
        .rotate_an = 1,                  \
        .emergency = 1,                    \
        .back_xx = 1,                    \
    }

#define BSP_EXT_IO_DEFAULT_LEVEL() {    \
        .bubble = 1,                \
        .water = 1,                   \
        .stretch = 1,                   \
        .draw = 1,                   \
        .rotate_ck = 1,                    \
        .rotate_an = 1,                  \
        .emergency = 1,                    \
        .back_xx = 1,                    \
    }

#define BSP_EXT_IO_SLEEP_LEVEL() {      \
        .bubble = 1,                \
        .water = 1,                   \
        .stretch = 1,                   \
        .draw = 1,                   \
        .rotate_ck = 1,                    \
        .rotate_an = 1,                  \
        .emergency = 1,                    \
        .back_xx = 1,                    \
    }

#endif
