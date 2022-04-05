#include "gui_setting.h"

#define CONT_NUM 3

enum EnumCont
{
    CONT_SOUND = 0,    //声音
    CONT_WIFI,         // WIFI
    CONT_CAMERA_STYLE, //照片风格
};

const char *setting_icons[] = {
    M_FONT(SETTING, 0),
    // M_FONT(SETTING, 1),
    ICON_SYMBOL_WIFI,
    M_FONT(SETTING, 2),
};

const char *special_effect[] = {
    "No Effect",
    "Negative",
    "GrayScale",
    "Red Tint",
    "Green Tint",
    "Blue Tint",
    "Sepia",
};

page_handler page_setting_handle;

lv_obj_t *lv_setting;
lv_obj_t *lv_conts[CONT_NUM];
lv_obj_t *lv_labels[CONT_NUM];
lv_obj_t *lv_arrows[CONT_NUM];

static short select_index;

static void arrows_clear()
{
    for (int i = 0; i < CONT_NUM; i++)
    {
        lv_obj_add_flag(lv_arrows[i], LV_OBJ_FLAG_HIDDEN);
    }
}

inline void arrow_show(uint8_t index)
{
    lv_obj_clear_flag(lv_arrows[index], LV_OBJ_FLAG_HIDDEN);
}

static void label_txt_init()
{
    //声音
    if (app_config.sound_state == 0)
        lv_label_set_text(lv_labels[0], "Off");
    else
        lv_label_set_text(lv_labels[0], "On");

    // WIFI
    if (app_config.wifi_state == 0)
        lv_label_set_text(lv_labels[1], "Off");
    else
        lv_label_set_text(lv_labels[1], "On");

    //照片风格
    lv_label_set_text(lv_labels[2], special_effect[app_config.camera_state]);
}

static void func_config()
{
    uint8_t len;
    switch (select_index)
    {
    case CONT_SOUND:
        app_config.sound_state = !app_config.sound_state;
        break;
    case CONT_WIFI:
        app_config.wifi_state = !app_config.wifi_state;
        break;
    case CONT_CAMERA_STYLE:
        len = sizeof(special_effect) / sizeof(special_effect[0]);
        app_config.camera_state++;
        if (app_config.camera_state >= len)
            app_config.camera_state = 0;

        controller_start_task(CF_CONFIG);
        vTaskDelay(200 / portTICK_RATE_MS);
        break;
    }

    label_txt_init();
}

static void event_key_handler(lv_event_t *e)
{
    switch (e->code)
    {
    case LV_EVENT_KEY:
    {
        const uint32_t key = lv_indev_get_key(lv_indev_get_act());
        if (key == LV_KEY_LEFT)
        {
            arrows_clear();
            select_index--;
            select_index = LV_MAX(select_index, 0);
            arrow_show(select_index);
        }
        else if (key == LV_KEY_RIGHT)
        {
            arrows_clear();
            select_index++;
            select_index = LV_MIN(select_index, CONT_NUM);

            if (select_index < CONT_NUM)
                arrow_show(select_index);
        }
        else if (key == LV_KEY_ENTER)
        {

            if (select_index == CONT_NUM)
            {
                //保存配置
                if (app_config_write() == ESP_OK)
                {
                    ESP_LOGI(GUI_TAG, "config save successful");
                }
                page_setting_handle(PAGE_MAIN);
            }
            else
            {
                func_config();
            }
        }
    }
    break;
    default:
        break;
    }
}

void page_setting_init(lv_obj_t *parent, page_handler handle)
{
    lv_setting = parent;
    page_setting_handle = handle;

    lv_group_add_obj(group, lv_setting);
    lv_obj_add_event_cb(lv_setting, event_key_handler, LV_EVENT_KEY, NULL);

    lv_obj_t *lv_title = lv_label_create(lv_setting);
    lv_label_set_text(lv_title, page_title[PAGE_SETTING]);
    lv_obj_align(lv_title, LV_ALIGN_TOP_MID, 0, 10);

    for (int i = 0; i < CONT_NUM; i++)
    {
        lv_conts[i] = lv_obj_create(lv_setting);
        lv_obj_set_size(lv_conts[i], 180, 45);

        lv_labels[i] = lv_label_create(lv_conts[i]);
        lv_label_set_text(lv_labels[i], "");
        lv_obj_align(lv_labels[i], LV_ALIGN_CENTER, 0, 0);

        lv_arrows[i] = lv_label_create(lv_conts[i]);
        lv_obj_add_flag(lv_arrows[i], LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(lv_arrows[i], "<-");
        lv_obj_align(lv_arrows[i], LV_ALIGN_RIGHT_MID, -10, 0);
    }

    lv_obj_align_to(lv_conts[CONT_SOUND], lv_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 25);
    lv_obj_align_to(lv_conts[CONT_WIFI], lv_conts[CONT_SOUND], LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_align_to(lv_conts[CONT_CAMERA_STYLE], lv_conts[CONT_WIFI], LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    //声音设置布局
    lv_obj_t *lv_sound_label = lv_label_create(lv_conts[CONT_SOUND]);
    lv_obj_add_style(lv_sound_label, &icon_style, 0);
    lv_label_set_text(lv_sound_label, setting_icons[CONT_SOUND]);

    //背光设置布局
    lv_obj_t *lv_wifi_label = lv_label_create(lv_conts[CONT_WIFI]);
    lv_obj_add_style(lv_wifi_label, &icon_style, 0);
    lv_label_set_text(lv_wifi_label, setting_icons[CONT_WIFI]);

    //照片风格设置布局
    lv_obj_t *lv_camera_label = lv_label_create(lv_conts[CONT_CAMERA_STYLE]);
    lv_obj_add_style(lv_camera_label, &icon_style, 0);
    lv_label_set_text(lv_camera_label, setting_icons[CONT_CAMERA_STYLE]);
}

void page_setting_focus()
{
    lv_group_focus_obj(lv_setting);
    select_index = 0;
    arrow_show(select_index);

    // app_config.sound_state = 0;
    // app_config.bl_state = 6;
    // app_config.camera_state = 0;

    label_txt_init();
}

void camera_config_handler(control_param_t *cpt)
{
    cpt->cfg_t->cfg_type = SPECIAL_EFFECT;
    cpt->cfg_t->val = app_config.camera_state;
    ESP_LOGI(GUI_TAG, "camera: type: %d; val: %d", SPECIAL_EFFECT, app_config.camera_state);
}
