#ifndef _GUI_CAMERA_H
#define _GUI_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_inc.h"

#define FUNC_COUNT 3

enum FUNC{
    FUNC_CAPTURE = 0,
    FUNC_QRCODE,
    FUNC_BACK,
    FUNC_QRBOX
};

typedef struct BPack {
    uint8_t sw_index : 2; //功能切换计数
    uint8_t qr_flag : 1;  //二维码开关标志
    uint8_t icon_anim_h : 5; //功能提示图标动画距离
}bpack_t;

void page_camera_init(lv_obj_t* parent, page_handler handle);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // !_GUI_CAMERA_H
