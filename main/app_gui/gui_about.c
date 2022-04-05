#include "gui_about.h"

page_handler page_about_handle;

lv_obj_t* lv_about;

static void event_key_handler(lv_event_t* e)
{
    switch (e->code)
    {
    case LV_EVENT_KEY:
    {
        const uint32_t key = lv_indev_get_key(lv_indev_get_act());
        if (key == LV_KEY_ENTER) {
            page_about_handle(PAGE_MAIN);
        }
    }
    break;
    default:
        break;
    }
}

void page_about_init(lv_obj_t* parent, page_handler handle)
{
    lv_about = parent;
    page_about_handle = handle;

    lv_group_add_obj(group, lv_about);
    lv_obj_add_event_cb(lv_about, event_key_handler, LV_EVENT_KEY, NULL);

    lv_obj_t* lv_title = lv_label_create(lv_about);
    lv_label_set_text(lv_title, page_title[PAGE_ABOUT]);
    lv_obj_align(lv_title, LV_ALIGN_TOP_MID, 0, 10);

    const char* data = "https://www.wiyixiao4.com/blog";
    lv_obj_t* lv_qr = lv_qrcode_create(lv_about, 100, lv_color_hex3(0x33f), lv_color_hex3(0xeef));
    lv_qrcode_update(lv_qr, data, strlen(data));
    lv_obj_align(lv_qr, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* lv_version = lv_label_create(lv_about);
    lv_label_set_text_fmt(lv_version, "LVGL %d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
    lv_obj_align_to(lv_version, lv_qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

void page_about_focus()
{
    lv_group_focus_obj(lv_about);
}
