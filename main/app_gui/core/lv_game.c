#include "lv_game.h"

typedef struct LvGame {
    lv_obj_t* lv_c;
    lv_draw_img_dsc_t img_dsc;
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_label_dsc_t label_dsc;
    lv_timer_t* lv_timer;
    key_left_handle left_cb;
    key_right_handle right_cb;
    key_enter_handle enter_cb;
    key_update_handle update_cb;
}lv_game_t;

lv_game_t game_obj;
lv_coord_t lv_game_w, lv_game_h;

static void event_key_handler(lv_event_t* e)
{
    switch (e->code)
    {
        case LV_EVENT_KEY:
        {
            const uint32_t key = lv_indev_get_key(lv_indev_get_act());
            if (key == LV_KEY_LEFT)
            {
                game_obj.left_cb();
            }
            else if (key == LV_KEY_RIGHT)
            {
                game_obj.right_cb();
            }
            else if (key == LV_KEY_ENTER)
            {
                game_obj.enter_cb();
            }
        }
            break;
        default:
            break;
    }
}

void lv_game_reg_keyleft(key_left_handle cb)
{
    game_obj.left_cb = cb;
}

void lv_game_reg_keyright(key_right_handle cb)
{
    game_obj.right_cb = cb;
}

void lv_game_reg_keyenter(key_enter_handle cb)
{
    game_obj.enter_cb = cb;
}

void lv_game_reg_update(key_update_handle cb)
{
    game_obj.update_cb = cb;
}

void lv_game_set_canvas(lv_obj_t* canvas)
{
    LV_LOG_USER("lv game init");
    game_obj.lv_c = canvas;
    lv_obj_add_event_cb(game_obj.lv_c, event_key_handler, LV_EVENT_ALL, NULL);
}


void lv_game_begin(uint8_t fps)
{
    lv_draw_img_dsc_init(&game_obj.img_dsc);
    lv_draw_rect_dsc_init(&game_obj.rect_dsc);
    lv_draw_label_dsc_init(&game_obj.label_dsc);
    game_obj.label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);

    lv_game_w = lv_obj_get_width(game_obj.lv_c);
    lv_game_h = lv_obj_get_height(game_obj.lv_c);

    game_obj.lv_timer = lv_timer_create(game_obj.update_cb, (200 / fps), NULL);
}

void lv_game_drawimg(lv_coord_t x, lv_coord_t y, const void* src)
{
    lv_canvas_draw_img(game_obj.lv_c,
        x,
        y,
        src,
        &game_obj.img_dsc);
}

void lv_game_drawtxt(lv_coord_t x, lv_coord_t y, lv_coord_t max_w, const char* txt)
{
    lv_canvas_draw_text(game_obj.lv_c, x, y, LV_SIZE_CONTENT, &game_obj.label_dsc, txt);
}

void lv_game_clear(lv_color_t color)
{
    //lv_color_white()
    lv_canvas_fill_bg(game_obj.lv_c,color, LV_OPA_COVER);
}

void lv_game_clear_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    lv_canvas_draw_rect(game_obj.lv_c, x, y, w, h, &game_obj.rect_dsc);
}

void lv_game_end()
{
    lv_timer_del(game_obj.lv_timer);
    lv_game_clear(lv_color_white());
}
