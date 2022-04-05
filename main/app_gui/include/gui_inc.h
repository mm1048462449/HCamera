#ifndef _GUI_INCLUDE_H
#define _GUI_INCLUDE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl/lvgl.h"
#include "lv_inc.h"
#include "hcamera_symbol_def.h"
#include "gui_handler.h"
#include <stdio.h>
#include <string.h>
#include "lcd_1inch28.h"
#include "i2c_slave.h"
#include "app_client.h"
#include "app_controller.h"
#include "app_config.h"
#include "app_wifi.h"
#include "img_converters.h"

#define M_FONT(TAG, INDEX) TAG##_SYMBOL_##INDEX

#define GUI_TAG "HCamera GUI"

#define GUI_WIDTH 240
#define GUI_HEIGHT 240

#define FRAME_WIDTH GUI_WIDTH
#define FRAME_HEIGHT GUI_HEIGHT
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT * LV_COLOR_DEPTH) / 8

#define ANIM_TIME 120
#define PAGE_COUNT 6

#define ARC_ADD_ANIM(arc)                                      \
    do                                                         \
    {                                                          \
        lv_anim_t a;                                           \
        lv_anim_init(&a);                                      \
        lv_anim_set_var(&a, arc);                              \
        lv_anim_set_exec_cb(&a, set_angle);                    \
        lv_anim_set_time(&a, 1000);                            \
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); \
        lv_anim_set_repeat_delay(&a, 500);                     \
        lv_anim_set_values(&a, 0, 100);                        \
        lv_anim_start(&a);                                     \
    } while (0)

    enum
    {
        PAGE_MAIN = 0,
        PAGE_CAMERA,
        PAGE_PICTURE,
        PAGE_GAME,
        PAGE_SETTING,
        PAGE_ABOUT
    };

    /**
     * @Fonts (Unicode)
     * 主页图标
     * Main:    f164
     * Camera:  f030
     * Picture: f03e
     * Game:    f11b
     * Setting: f013
     * About:   f05a
     *
     * 视频页图标
     * Capture: f083
     * QRcode:  f029
     * Back:    f0e2
     *
     * 设置页图标
     * Sound:   f001
     * BL:      f042
     * Camera:  f030
     *
     * 其他
     * Wifi:    f1eb
     *
     * 0xf164,0xf030,0xf03e,0xf11b,0xf013,0xf05a,
     * 0xf083,0xf029,0xf0e2,
     * 0xf001,0xf042,0xf1eb,
     */
    LV_FONT_DECLARE(hcamera_fonts_18);
    LV_IMG_DECLARE(bingdundun);
    LV_IMG_DECLARE(picture);

    static const char *page_title[] = {
        "Main",
        "Camera",
        "Picture",
        "Game",
        "Setting",
        "About"};

    static lv_img_dsc_t jpeg_dsc = {
        .header.cf = LV_IMG_CF_TRUE_COLOR,
        .header.always_zero = 0,
        .header.reserved = 0,
        .header.w = FRAME_WIDTH,
        .header.h = FRAME_HEIGHT,
    };

    extern lv_group_t *group;
    extern lv_style_t icon_style;
    extern lv_style_t bg_white_style;

    // Handler
    typedef void (*page_handler)(void *arg);
    typedef void (*page_focus_handler)();
    typedef void (*page_init_handler)(lv_obj_t *parent, page_handler handle);

    // Page focus method
    void page_main_focus();
    void page_camera_focus();
    void page_picture_focus();
    void page_game_focus();
    void page_setting_focus();
    void page_about_focus();

    // Static method
    static inline void init_outline_shadow_style(lv_style_t *style, lv_opa_t opa, lv_palette_t color, uint8_t w)
    {
        lv_style_init(style);

        // Set background & radius
        lv_style_set_radius(style, 5);
        lv_style_set_bg_opa(style, opa);
        lv_style_set_bg_color(style, lv_palette_lighten(LV_PALETTE_GREY, 1));

        // Add shadow
        lv_style_set_shadow_width(style, w);
        lv_style_set_shadow_color(style, lv_palette_main(color));
    }

    static inline void init_background_style(lv_style_t *style, lv_opa_t opa, lv_color_t color)
    {
        lv_style_init(style);
        lv_style_set_bg_opa(style, opa);
        lv_style_set_bg_color(style, color);
    }

    static inline void init_font_style(lv_style_t *style, lv_color_t color, lv_font_t *font)
    {
        lv_style_init(style);

        lv_style_set_text_font(style, font);
        lv_style_set_text_color(style, color);
    }

    static inline void set_angle(void *obj, int32_t v)
    {
        lv_arc_set_value((lv_obj_t *)obj, (int32_t)v);
    }

    static inline void dialog_loading(lv_obj_t *root, bool state)
    {
        static lv_style_t style;
        static lv_obj_t *arc_loading;

        if (state)
        {
            lv_style_init(&style);
            lv_style_set_arc_width(&style, 8);

            arc_loading = lv_obj_create(root);
            lv_obj_set_size(arc_loading, 100, 100);
            lv_obj_center(arc_loading);

            lv_obj_t *arc = lv_arc_create(arc_loading);
            lv_obj_set_size(arc, 60, 60);
            lv_arc_set_rotation(arc, 270);
            lv_arc_set_bg_angles(arc, 0, 360);
            lv_obj_remove_style(arc, NULL, LV_PART_KNOB); /*Be sure the knob is not displayed*/
            lv_obj_add_style(arc, &style, LV_PART_MAIN);
            lv_obj_add_style(arc, &style, LV_PART_INDICATOR);
            lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE); /*To not allow adjusting by click*/
            lv_obj_center(arc);

            ARC_ADD_ANIM(arc);
        }

        if (!state && arc_loading)
        {
            // lv_obj_del(arc_loading);
            lv_obj_del_async(arc_loading);
            arc_loading = NULL;
        }
    }
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // !_GUI_INCLUDE_H
