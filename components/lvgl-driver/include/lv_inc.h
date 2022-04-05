#ifndef _COM_GUI_LVGL_H
#define _COM_GUI_LVGL_H

//LVGL 8.2.0

#ifdef __cplusplus
extern "C"
{
#endif

#include "sdkconfig.h"

#include "lv_conf.h"
#include "lvgl/lvgl.h"

/* lvgl input device includes */
#include "lv_port_indev.h"

/* lvgl display device includes */
#include "lv_port_disp.h"

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize LittlevGL GUI 
 */
void lvgl_init();

#ifdef __cplusplus
}
#endif

#endif /* _COM_GUI_LVGL_H */