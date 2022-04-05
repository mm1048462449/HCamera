#ifndef _GUI_HANDLER_H
#define _GUI_HANDLER_H

#include "app_controller.h"

/**
 * 具体到某个页面中实现
 * Camera 
 */
void camera_capture_handler(control_param_t *cpt);
void camera_video_handler(control_param_t *cpt);
void camera_config_handler(control_param_t *cpt);
void camera_qrcode_handler(control_param_t *cpt);
void camera_stream_handler(control_param_t *cpt);

#endif