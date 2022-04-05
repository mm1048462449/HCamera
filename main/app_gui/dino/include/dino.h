#ifndef _DINO_H
#define _DINO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "lv_game.h"
#include "lvgl/lvgl.h"

LV_IMG_DECLARE(berd);
LV_IMG_DECLARE(berd1);
LV_IMG_DECLARE(cactus_big);
LV_IMG_DECLARE(cactus_small);
LV_IMG_DECLARE(cactus_small_many);
LV_IMG_DECLARE(dino_dead);
LV_IMG_DECLARE(dino_logo);
LV_IMG_DECLARE(dino_jump);
LV_IMG_DECLARE(dino_run_0);
LV_IMG_DECLARE(dino_run_1);

void dino_init(lv_obj_t* canvas, void(*exit_handle)(void* arg));
void dino_run();
void dino_exit();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // !_DINO_H
