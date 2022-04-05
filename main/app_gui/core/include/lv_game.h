#ifndef _LV_GAME_H
#define _LV_GAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "lvgl/lvgl.h"

typedef void(*key_left_handle)();
typedef void(*key_right_handle)();
typedef void(*key_enter_handle)();
typedef void(*key_update_handle)(lv_timer_t* timer);

extern lv_coord_t lv_game_w;
extern lv_coord_t lv_game_h;

void lv_game_reg_keyleft(key_left_handle cb);
void lv_game_reg_keyright(key_right_handle cb);
void lv_game_reg_keyenter(key_enter_handle cb);
void lv_game_reg_update(key_update_handle cb);
void lv_game_set_canvas(lv_obj_t* canvas);

void lv_game_begin(uint8_t fps);
void lv_game_end();

void lv_game_drawimg(lv_coord_t x, lv_coord_t y, const void* src);
void lv_game_drawtxt(lv_coord_t x, lv_coord_t y, lv_coord_t max_w, const char* txt);
void lv_game_clear(lv_color_t color);
void lv_game_clear_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // !_LV_GAME_H
