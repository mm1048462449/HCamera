#include "gui_game.h"
#include "dino.h"

#define CANVAS_WIDTH 200
#define CANVAS_HEIGHT 130

page_handler page_game_handle;
var_pack_t vpack_t = {
    .list_item = 0,
};

lv_obj_t* lv_game;
lv_obj_t* lv_game_list;
lv_obj_t* lv_canvas;
lv_obj_t* lv_current_btn;

static void game_exit_cb(void* arg)
{
    int flag = *((int*)arg);
    LV_LOG_USER("exit flag: %d", flag);

    lv_obj_add_flag(lv_canvas, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(lv_game_list, LV_OBJ_FLAG_HIDDEN);
    lv_group_focus_obj(lv_game);
}

static void game_list_init()
{
    uint32_t i;
    for (i = 0; i < lv_obj_get_child_cnt(lv_game_list); i++) {
        lv_obj_t* child = lv_obj_get_child(lv_game_list, i);
        lv_obj_clear_state(child, LV_STATE_CHECKED);
    }
}

static void game_list_up()
{
    if (lv_current_btn == NULL) return;
    uint32_t index = lv_obj_get_index(lv_current_btn);
    if (index <= 0) return;

    game_list_init();
    lv_current_btn = lv_obj_get_child(lv_game_list, --index);
    lv_obj_add_state(lv_current_btn, LV_STATE_CHECKED);

    vpack_t.list_item = index;
}

static void game_list_down()
{
    if (lv_current_btn == NULL) return;
    uint32_t index = lv_obj_get_index(lv_current_btn);
    if (index >= lv_obj_get_child_cnt(lv_game_list)-1) return;

    game_list_init();
    lv_current_btn = lv_obj_get_child(lv_game_list, ++index);
    lv_obj_add_state(lv_current_btn, LV_STATE_CHECKED);

    vpack_t.list_item = index;

}

static void event_key_handler(lv_event_t* e)
{
    switch (e->code)
    {
    case LV_EVENT_KEY:
    {
        const uint32_t key = lv_indev_get_key(lv_indev_get_act());
        if (key == LV_KEY_LEFT) {
           game_list_up(); //未选择游戏时进行列表项移动
        }
        else if (key == LV_KEY_RIGHT) {
            game_list_down();
        } 
        else if (key == LV_KEY_ENTER) {
            LV_LOG_USER("index %d", vpack_t.list_item);
            if (vpack_t.list_item == ITEM_BACK)
            {
                page_game_handle(PAGE_MAIN);
            }
            else {

                //隐藏列表
                lv_obj_add_flag(lv_game_list, LV_OBJ_FLAG_HIDDEN);

                //切换焦点
                lv_obj_clear_flag(lv_canvas, LV_OBJ_FLAG_HIDDEN);
                lv_group_focus_obj(lv_canvas);
            }

            if (vpack_t.list_item == ITEM_DINO)
            {
                dino_run();
            }
        }
    }
    break;
    default:
        break;
    }
}

static void list_add(lv_obj_t* list, const char* txt)
{
    lv_obj_t* btn = lv_btn_create(list);
    lv_obj_set_width(btn, lv_pct(100));

    lv_obj_t* lab = lv_label_create(btn);
    lv_label_set_text(lab, txt);
}

void page_game_init(lv_obj_t* parent, page_handler handle)
{
    lv_game = parent;
    page_game_handle = handle;

    lv_group_add_obj(group, lv_game);
    lv_obj_add_event_cb(lv_game, event_key_handler, LV_EVENT_KEY, NULL);

    lv_obj_t* lv_title = lv_label_create(lv_game);
    lv_label_set_text(lv_title, page_title[PAGE_GAME]);
    lv_obj_align(lv_title, LV_ALIGN_TOP_MID, 0, 10);

    //创建列表
    lv_game_list = lv_list_create(lv_game);
    lv_obj_set_size(lv_game_list, 200, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_row(lv_game_list, 5, 0);
    lv_obj_center(lv_game_list);

    list_add(lv_game_list, "Dino");
    list_add(lv_game_list, "Back");

    lv_current_btn = lv_obj_get_child(lv_game_list, 0);
    lv_obj_add_state(lv_current_btn, LV_STATE_CHECKED);

    //创建画布
    EXT_RAM_ATTR static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)]; //申请外部内存
    lv_canvas = lv_canvas_create(lv_game);
    lv_group_add_obj(group, lv_canvas);
    lv_canvas_set_buffer(lv_canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(lv_canvas);

    lv_canvas_fill_bg(lv_canvas, lv_color_white(), LV_OPA_COVER);
    lv_obj_add_flag(lv_canvas, LV_OBJ_FLAG_HIDDEN);

    //初始化游戏
    dino_init(lv_canvas, game_exit_cb);
}

void page_game_focus()
{
    vpack_t.list_item = 0;
    lv_group_focus_obj(lv_game);

}
