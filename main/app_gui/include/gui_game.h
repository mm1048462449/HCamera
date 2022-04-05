#ifndef _GUI_GAME_H
#define _GUI_GAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_inc.h"

enum LIST_ITEM{
    ITEM_DINO = 0,
    ITEM_BACK
};

typedef struct VarPack {
    uint8_t list_item : 3;
}var_pack_t;

void page_game_init(lv_obj_t* parent, page_handler handle);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // !_GUI_GAME_H
