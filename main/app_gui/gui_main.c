#include "gui_main.h"

static lv_anim_timeline_t *anim_timeline = NULL;
static lv_style_t obj_style;
static const lv_coord_t obj_width = 56;
static const lv_coord_t obj_height = 52;
static uint8_t _show = 0;

const char *main_icons[] = {
    M_FONT(MAIN, 0),
    M_FONT(MAIN, 1),
    M_FONT(MAIN, 2),
    M_FONT(MAIN, 3),
    M_FONT(MAIN, 4),
    M_FONT(MAIN, 5)};

page_handler page_main_handle;

lv_obj_t *lv_main;
lv_obj_t *lv_objs[PAGE_COUNT];
lv_obj_t *lv_img_wifi;

static void set_width(void *var, int32_t v)
{
    lv_obj_set_width((lv_obj_t *)var, v);
}

static void set_height(void *var, int32_t v)
{
    lv_obj_set_height((lv_obj_t *)var, v);
}

static void anim_timeline_create(void)
{
    anim_timeline = lv_anim_timeline_create();
    uint16_t t_duration = 350;

    for (int i = 1; i < PAGE_COUNT; i++)
    {
        lv_anim_t a1;
        lv_anim_init(&a1);
        lv_anim_set_var(&a1, lv_objs[i]);
        lv_anim_set_values(&a1, 0, obj_width);
        lv_anim_set_early_apply(&a1, false);
        lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)set_width);
        lv_anim_set_path_cb(&a1, lv_anim_path_overshoot);
        lv_anim_set_time(&a1, t_duration);

        lv_anim_t a2;
        lv_anim_init(&a2);
        lv_anim_set_var(&a2, lv_objs[i]);
        lv_anim_set_values(&a2, 0, obj_height);
        lv_anim_set_early_apply(&a2, false);
        lv_anim_set_exec_cb(&a2, (lv_anim_exec_xcb_t)set_height);
        lv_anim_set_path_cb(&a2, lv_anim_path_ease_out);
        lv_anim_set_time(&a2, t_duration);

        lv_anim_timeline_add(anim_timeline, i, &a1);
        lv_anim_timeline_add(anim_timeline, i, &a2);
    }
}

static void event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    int id = (int)lv_obj_get_user_data(target);
    LV_LOG_USER("Clicked %d", id);

    if (id == PAGE_MAIN)
    {
        LV_LOG_USER("Main page");
        return;
    }

    lv_anim_timeline_set_reverse(anim_timeline, true);
    lv_anim_timeline_start(anim_timeline);
    page_main_handle(id); //跳转页面
}

static void event_key_handler(lv_event_t *e)
{
    static short _index = 0;
    LV_LOG_USER("code", e->code);

    switch (e->code)
    {
    case LV_EVENT_KEY:
    {
        const uint32_t key = lv_indev_get_key(lv_indev_get_act());
        if (key == LV_KEY_LEFT)
        {
            if (_show == 0)
                return; //隐藏状态不切换按钮
            _index--;
            _index = _index <= 0 ? 0 : _index;

            if (_index >= 0)
                lv_obj_remove_style(lv_objs[_index + 1], &obj_style, 0);
            lv_obj_add_style(lv_objs[_index], &obj_style, 0);
        }
        else if (key == LV_KEY_RIGHT)
        {
            if (_show == 0)
                return; //隐藏状态不切换按钮
            _index++;
            _index = _index >= PAGE_COUNT ? (PAGE_COUNT - 1) : _index;

            if (_index > 0)
                lv_obj_remove_style(lv_objs[_index - 1], &obj_style, 0);
            lv_obj_add_style(lv_objs[_index], &obj_style, 0);
        }
        else if (key == LV_KEY_ENTER)
        {
            if (_index == PAGE_MAIN)
            {
                controller_sound_task();
                if (_show)
                {
                    lv_anim_timeline_set_reverse(anim_timeline, true);
                    lv_anim_timeline_start(anim_timeline);
                }
                else
                {
                    lv_anim_timeline_set_reverse(anim_timeline, false);
                    lv_anim_timeline_start(anim_timeline);
                }
                _show = !_show;
                return;
            }

            page_main_handle(_index); //跳转页面
        }
    }
    break;
    default:
        break;
    }
}

static void add_btn(lv_obj_t *root, uint8_t index)
{
    lv_objs[index] = lv_obj_create(root);
    lv_obj_add_event_cb(lv_objs[index], event_cb, LV_EVENT_CLICKED, NULL); //鼠标控制
    lv_obj_set_user_data(lv_objs[index], (void *)index);                   //设置用户数据，用于区分按钮
    lv_obj_set_size(lv_objs[index], obj_width, obj_height);

    lv_obj_t *label = lv_label_create(lv_objs[index]);
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(label, &icon_style, 0);     //设置字体样式
    lv_label_set_text(label, main_icons[index]); //设置字体
    lv_obj_center(label);
}

void page_main_init(lv_obj_t *parent, page_handler handle)
{
    static lv_style_t main_style;
    lv_main = parent;
    page_main_handle = handle;

    lv_style_init(&main_style);
    lv_style_set_bg_opa(&main_style, LV_OPA_10);

    lv_group_add_obj(group, lv_main);
    lv_obj_add_event_cb(lv_main, event_key_handler, LV_EVENT_KEY, NULL);

    //创建主页背景
    lv_obj_t *lv_img_bg = lv_img_create(lv_main);
    lv_img_set_src(lv_img_bg, &bingdundun);
    lv_obj_align(lv_img_bg, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(lv_img_bg, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    //创建wifi连接图标
    lv_img_wifi = lv_img_create(lv_main);
    lv_img_set_src(lv_img_wifi, &ICON_SYMBOL_WIFI);
    lv_obj_align(lv_img_wifi, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_size(lv_img_wifi, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_flag(lv_img_wifi, LV_OBJ_FLAG_HIDDEN);

    //创建按钮布局
    lv_obj_t *lv_btn_cont = lv_obj_create(lv_main);
    lv_obj_set_size(lv_btn_cont, GUI_WIDTH, GUI_HEIGHT);
    lv_obj_add_style(lv_btn_cont, &main_style, 0);
    lv_obj_center(lv_btn_cont);

    lv_obj_set_flex_flow(lv_btn_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_btn_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    //创建主页按钮
    for (int i = 0; i < PAGE_COUNT; i++)
    {
        add_btn(lv_btn_cont, i);
    }

    //初始化按钮边框样式
    init_outline_shadow_style(&obj_style, LV_OPA_0, LV_PALETTE_BLUE, obj_width - 12);
    lv_obj_add_style(lv_objs[0], &obj_style, 0);

    if (!anim_timeline)
    {
        anim_timeline_create();
    }

    lv_anim_timeline_set_reverse(anim_timeline, true); //隐藏
    lv_anim_timeline_start(anim_timeline);
}

void lv_timer_cb(lv_timer_t *lv_timer)
{
    //等待15s
    static int t = 15;
    ESP_LOGI(GUI_TAG, "%d", t);
    if (client_check_connected())
    {
        ESP_LOGI(GUI_TAG, "Wifi connected");
        dialog_loading(lv_main, false);
        lv_obj_clear_flag(lv_img_wifi, LV_OBJ_FLAG_HIDDEN);
        lv_timer_del(lv_timer);
    }
    else
    {
        t--;
        if (t <= 0)
        {
            t = 15;
            dialog_loading(lv_main, false);
            lv_obj_add_flag(lv_img_wifi, LV_OBJ_FLAG_HIDDEN);
            lv_timer_del(lv_timer);
            ESP_LOGI(GUI_TAG, "Wifi connected timeout");
        }
    }
}

void page_main_focus()
{
    lv_group_focus_obj(lv_main);

    //检测wifi是否连接，未连接开启一个任务等待连接
    if (!client_check_connected() && app_config.wifi_state)
    {
        ESP_LOGE(GUI_TAG, "Wifi Lost");
        dialog_loading(lv_main, true);
        lv_timer_t *lv_timer = lv_timer_create(lv_timer_cb, 1000, NULL);
    }
}
