#include "gui_picture.h"

#define DOWNLOAD_DEFAULT_TXT "#ff0000 Downloading...#\n#0000ff 0 %#"

page_handler page_picture_handle;

lv_obj_t *lv_picture;
lv_obj_t *lv_file_list;
lv_obj_t *lv_page_label;
lv_obj_t *lv_warn_label;
lv_obj_t *lv_img;

uint8_t *out_img = NULL;
static short list_index = 0;
EXT_RAM_ATTR static uint16_t img_temp[GUI_WIDTH * GUI_HEIGHT];

static void list_clear();
static void list_add(const char *txt);

static int is_jpg(const uint8_t *raw_data)
{
    const uint8_t jpg_signature[] = {0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46};
    return memcmp(jpg_signature, raw_data, sizeof(jpg_signature)) == 0;
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
            lv_obj_t *item = lv_obj_get_child(lv_file_list, list_index);
            lv_obj_clear_state(item, LV_STATE_CHECKED);

            list_index--;
            list_index = LV_MAX(list_index, 0);
            item = lv_obj_get_child(lv_file_list, list_index);
            lv_obj_add_state(item, LV_STATE_CHECKED);
        }
        else if (key == LV_KEY_RIGHT)
        {
            lv_obj_t *item = lv_obj_get_child(lv_file_list, list_index);
            lv_obj_clear_state(item, LV_STATE_CHECKED);

            list_index++;
            list_index = LV_MIN(list_index, lv_obj_get_child_cnt(lv_file_list) - 1);
            item = lv_obj_get_child(lv_file_list, list_index);
            lv_obj_add_state(item, LV_STATE_CHECKED);
        }
        else if (key == LV_KEY_ENTER)
        {
            if (list_index == 0)
            {
                //返回
                if (out_img)
                {
                    free(out_img);
                    out_img = NULL;
                }
                page_picture_handle(PAGE_MAIN);
            }
            else if (list_index == 1)
            {
                //上一页
                list_clear();

                file_req.pn = LV_MAX(file_res.page_num--, 1);
                client_task_begin(TASK_FILE_LIST);
            }
            else if (list_index == lv_obj_get_child_cnt(lv_file_list) - 1)
            {
                //下一页
                list_clear();

                file_req.pn = LV_MIN(file_res.page_num++, file_res.page_count);
                client_task_begin(TASK_FILE_LIST);
            }
            else
            {
                if (!lv_obj_has_flag(lv_img, LV_OBJ_FLAG_HIDDEN))
                {
                    //关闭图片
                    lv_obj_add_flag(lv_img, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    //查看图片
                    lv_obj_t *btn = lv_obj_get_child(lv_file_list, list_index);
                    lv_obj_t *label = lv_obj_get_child(btn, 0);
                    sprintf(file_req.file, "%s", lv_label_get_text(label));
                    client_task_begin(TASK_FILE_DOWNLOAD);

                    lv_obj_add_flag(lv_img, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(lv_warn_label, LV_OBJ_FLAG_HIDDEN);
                }
            }
        }
    }
    break;
    default:
        break;
    }
}

/**
 * 清除对象导致程序崩溃
 * 使用文本替换，不清除对象
 */
// static void list_clear()
// {
//     uint32_t len = lv_obj_get_child_cnt(lv_file_list);
//     if (len > 3)
//     {
//         lv_obj_t *child;

//         for (int i = len - 2; i >= 2; i--)
//         {
//             child = lv_obj_get_child(lv_file_list, i);
//             lv_obj_clean(child);
//             lv_obj_del(child);
//         }

//         for (int i = 0; i < lv_obj_get_child_cnt(lv_file_list); i++)
//         {
//             child = lv_obj_get_child(lv_file_list, i);
//             lv_obj_clear_state(child, LV_STATE_CHECKED);
//         }

//         list_index = 0;
//         child = lv_obj_get_child(lv_file_list, list_index);
//         lv_obj_add_state(child, LV_STATE_CHECKED);
//     }
// }

static void list_clear()
{
    uint32_t len = lv_obj_get_child_cnt(lv_file_list);
    if (len > 3)
    {
        for (int i = len - 2; i >= 2; i--)
        {
            lv_obj_t *btn = lv_obj_get_child(lv_file_list, i);
            lv_obj_t *label = lv_obj_get_child(btn, 0);
            lv_label_set_text(label, "");
        }
    }
}

static void list_add(const char *txt)
{
    lv_obj_t *obj;
    lv_obj_t *label;

    /*Add items to the column*/
    obj = lv_btn_create(lv_file_list);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);

    label = lv_label_create(obj);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(label, txt);
    lv_obj_center(label);

    lv_obj_add_flag(lv_file_list, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    uint32_t len = lv_obj_get_child_cnt(lv_file_list);
    if (len > 3)
    {
        //修改新添加列表项索引
        lv_obj_move_to_index(obj, len - 2);
    }
}

static void list_info_callback()
{
    lv_label_set_text_fmt(lv_page_label, "Page:%d/%d", file_res.page_num, file_res.page_count);
}

static void list_txt_callback(uint8_t index, const char *name)
{
    // ESP_LOGI(GUI_TAG, "%d:%s", index, name);
    index += 2; // BACK && PREV
    lv_obj_t *btn = lv_obj_get_child(lv_file_list, index);
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    lv_label_set_text(label, name);
}

static void req_prog_callback(float percent)
{
    ESP_LOGI(GUI_TAG, "%.2f", percent);
    char f_val[7];
    sprintf(f_val, "%.2f %%", percent);
    lv_label_set_text_fmt(lv_warn_label, "#ff0000 Downloading...#\n#0000ff %s#", f_val);
}

static void req_finish_callback(bool state)
{
    if (file_req.task_type == TASK_FILE_DOWNLOAD)
    {
        ESP_LOGI(GUI_TAG, "%d", file_res.file_len);
        lv_label_set_text(lv_warn_label, DOWNLOAD_DEFAULT_TXT);
        lv_obj_add_flag(lv_warn_label, LV_OBJ_FLAG_HIDDEN);

        if (file_res.file_len <= 0)
            return;

        //转换图片
        if (out_img && is_jpg(file_res.file_buf))
        {
            ESP_LOGI(GUI_TAG, "File is jpeg");
            jpg2rgb565(file_res.file_buf, file_res.file_len, out_img, JPG_SCALE_NONE);

            int index = 0;
            for (int i = 0; i < FRAME_SIZE; i += 2)
            {

                img_temp[index] = (*(out_img + i) << 8 | *(out_img + i + 1));
                index++;
            }

            jpeg_dsc.data_size = FRAME_SIZE;
            jpeg_dsc.data = img_temp;
            lv_img_set_src(lv_img, &jpeg_dsc);
            lv_obj_clear_flag(lv_img, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void page_picture_init(lv_obj_t *parent, page_handler handle)
{
    lv_picture = parent;
    page_picture_handle = handle;

    lv_obj_t *lv_title = lv_label_create(lv_picture);
    lv_label_set_text(lv_title, page_title[PAGE_PICTURE]);
    lv_obj_align(lv_title, LV_ALIGN_TOP_MID, 0, 10);

    //文件列表
    lv_file_list = lv_obj_create(lv_picture);
    lv_obj_set_size(lv_file_list, 200, 150);
    lv_obj_center(lv_file_list);
    lv_obj_set_flex_flow(lv_file_list, LV_FLEX_FLOW_COLUMN);

    lv_group_add_obj(group, lv_file_list);
    lv_obj_add_event_cb(lv_file_list, event_key_handler, LV_EVENT_KEY, NULL);

    //添加控制按钮
    list_add("Back");
    list_add("Prev");
    list_add("More...");

    list_add("");
    list_add("");
    list_add("");
    list_add("");
    list_add("");
    list_add("");

    //页标签
    lv_page_label = lv_label_create(lv_picture);
    lv_label_set_text(lv_page_label, "Page:1/1");
    lv_obj_align_to(lv_page_label, lv_file_list, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    //图片显示
    lv_img = lv_img_create(lv_picture);
    lv_img_set_src(lv_img, &picture);
    lv_obj_align(lv_img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(lv_img, LV_OBJ_FLAG_HIDDEN);

    //下载提示
    lv_warn_label = lv_label_create(lv_picture);
    lv_label_set_recolor(lv_warn_label, true);
    lv_label_set_text(lv_warn_label, DOWNLOAD_DEFAULT_TXT);
    lv_obj_set_style_text_align(lv_warn_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_style(lv_warn_label, &bg_white_style, 0);
    lv_obj_set_size(lv_warn_label, 140, LV_SIZE_CONTENT);
    lv_obj_center(lv_warn_label);
    lv_obj_add_flag(lv_warn_label, LV_OBJ_FLAG_HIDDEN);

    client_req_cb.list_txt = list_txt_callback;
    client_req_cb.list_info = list_info_callback;
    client_req_cb.req_prog = req_prog_callback;
}

void page_picture_focus()
{
    lv_group_focus_obj(lv_file_list);
    // lv_group_focus_obj(lv_picture);
    client_req_cb.req_finish = req_finish_callback;

    lv_obj_t *item = lv_obj_get_child(lv_file_list, 0);
    lv_obj_add_state(item, LV_STATE_CHECKED);

    if (!out_img)
    {
        out_img = (uint8_t *)malloc((GUI_WIDTH * GUI_HEIGHT * 2) * sizeof(uint8_t));
    }

    //请求文件列表
    client_task_begin(TASK_FILE_LIST);
}
