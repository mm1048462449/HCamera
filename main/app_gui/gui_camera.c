#include "gui_camera.h"

#define UPLOAD_LABEL_TXT "#ff0000 Uploading#\n#0000ff ...#"
#define QRDECODING_LABEL_TXT "#ff0000 QR Decoding#\n#0000ff ...#"

bpack_t m_bpack = {
    .sw_index = 0,
    .qr_flag = 0,
    .icon_anim_h = 10};

page_handler page_camera_handle;

lv_obj_t *lv_camera;
lv_obj_t *lv_info;
lv_obj_t *lv_qr_box;
lv_obj_t *lv_img_stream;
lv_obj_t *lv_warn_label;

const char *infos[] = {
    M_FONT(CAMERA, 0),
    M_FONT(CAMERA, 1),
    M_FONT(CAMERA, 2)};

static void req_finish_callback(bool state);

static void anim_x_cb(void *var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void anim_out_cb(lv_anim_t *a)
{
    lv_anim_del(a, NULL);
    lv_obj_set_y(lv_info, -lv_obj_get_height(lv_info));
}

static void anim_in_cb(lv_anim_t *a)
{
    uint8_t user_data = a->user_data;
    lv_obj_t *obj = (user_data == FUNC_QRBOX) ? NULL : lv_info;
    lv_anim_del(a, NULL);

    if (obj == NULL)
    {
        return;
    }

    lv_anim_t a2;
    lv_anim_init(&a2);
    lv_anim_set_var(&a2, obj);
    lv_anim_set_values(&a2, a->end_value, -lv_obj_get_height(obj)); //隐藏
    lv_anim_set_user_data(&a2, user_data);
    lv_anim_set_time(&a2, 150);
    lv_anim_set_delay(&a2, 220);
    lv_anim_set_exec_cb(&a2, anim_x_cb);
    lv_anim_set_ready_cb(&a2, anim_out_cb);
    lv_anim_set_path_cb(&a2, lv_anim_path_linear);
    lv_anim_start(&a2);
}

static void func_sw_cb(uint8_t n)
{
    lv_obj_t *obj = NULL;
    uint8_t end = m_bpack.icon_anim_h;
    if (n != FUNC_QRBOX)
    {
        obj = lv_info;
        lv_label_set_text(lv_info, infos[n]);
    }
    else
    {
        obj = lv_qr_box;
        end = (GUI_HEIGHT - lv_obj_get_height(obj)) >> 1;
        lv_obj_clear_flag(lv_qr_box, LV_OBJ_FLAG_HIDDEN);
    }

    //执行动画
    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, obj);
    lv_anim_set_values(&a1, -lv_obj_get_height(obj), end); //显示
    lv_anim_set_user_data(&a1, (void *)n);                 //设置用户数据
    lv_anim_set_time(&a1, 200);
    lv_anim_set_delay(&a1, 50);
    lv_anim_set_exec_cb(&a1, anim_x_cb);
    lv_anim_set_ready_cb(&a1, anim_in_cb);
    lv_anim_set_path_cb(&a1, lv_anim_path_overshoot);
    lv_anim_start(&a1);
}

static void show_qr_result(const char *txt)
{
    lv_obj_t *text = lv_msgbox_get_text(lv_qr_box);
    bool has_txt = txt && strlen(txt) > 0;

    if (has_txt && text != NULL)
    {
        lv_label_set_text(text, txt);
    }

    //设置弹出动画
    func_sw_cb(FUNC_QRBOX);
    m_bpack.qr_flag = 1;
}

static void event_key_handler(lv_event_t *e)
{
    switch (e->code)
    {
    case LV_EVENT_KEY:
    {
        const uint32_t key = lv_indev_get_key(lv_indev_get_act());
        if (key == LV_KEY_RIGHT)
        {
            if (++m_bpack.sw_index >= FUNC_COUNT)
            {
                m_bpack.sw_index = FUNC_CAPTURE;
            }
            func_sw_cb(m_bpack.sw_index); //功能切换

            LV_LOG_USER("index %d", m_bpack.sw_index);
        }
        else if (key == LV_KEY_ENTER)
        {
            switch (m_bpack.sw_index)
            {
            case FUNC_BACK:
                m_bpack.sw_index = 0;
                m_bpack.qr_flag = 0;

                //关闭视频流任务
                controller_stop_task(CF_VIDEO);
                page_camera_handle(PAGE_MAIN);
                break;
            case FUNC_CAPTURE:
                //wifi未启用时禁止拍照
                if (!app_config.wifi_state)
                    break;
                controller_stop_task(CF_VIDEO);
                controller_start_task(CF_CAPTURE);
                break;
            case FUNC_QRCODE:
                // wifi启用时禁止扫码
                if (app_config.wifi_state)
                    break;
                if (m_bpack.qr_flag == 0)
                {
                    //先停止视频流任务
                    controller_stop_task(CF_VIDEO);
                    //开始二维码扫描任务
                    controller_start_task(CF_QRCODE);

                    lv_label_set_text(lv_warn_label, QRDECODING_LABEL_TXT);
                    lv_obj_clear_flag(lv_warn_label, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    //关闭二维码弹窗
                    lv_obj_t *text = lv_msgbox_get_text(lv_qr_box);
                    lv_label_set_text(text, "null");
                    lv_obj_set_y(lv_qr_box, -lv_obj_get_height(lv_qr_box));
                    m_bpack.qr_flag = 0;
                }
                break;
            }
        }
    }
    break;
    default:
        break;
    }
}

void page_camera_init(lv_obj_t *parent, page_handler handle)
{
    lv_camera = parent;
    page_camera_handle = handle;

    lv_group_add_obj(group, lv_camera);
    lv_obj_add_event_cb(lv_camera, event_key_handler, LV_EVENT_KEY, NULL);

    //视频流显示
    lv_img_stream = lv_img_create(lv_camera);
    lv_obj_align(lv_img_stream, LV_ALIGN_CENTER, 0, 0);

    //功能提示图标
    lv_info = lv_label_create(lv_camera);
    lv_obj_add_style(lv_info, &icon_style, 0); //设置字体样式
    lv_label_set_text(lv_info, infos[0]);
    lv_obj_align(lv_info, LV_ALIGN_TOP_MID, 0, m_bpack.icon_anim_h);

    //二维码结果弹窗
    lv_qr_box = lv_msgbox_create(lv_camera, "QRCode", "null", NULL, false);
    lv_obj_add_flag(lv_qr_box, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(lv_qr_box, 180, LV_SIZE_CONTENT);
    lv_obj_align(lv_qr_box, LV_ALIGN_TOP_MID, 0, 0);

    //上传提示
    lv_warn_label = lv_label_create(lv_camera);
    lv_label_set_recolor(lv_warn_label, true);
    lv_label_set_text(lv_warn_label, "");
    lv_obj_set_style_text_align(lv_warn_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_style(lv_warn_label, &bg_white_style, 0);
    lv_obj_set_size(lv_warn_label, 120, LV_SIZE_CONTENT);
    lv_obj_center(lv_warn_label);
    lv_obj_add_flag(lv_warn_label, LV_OBJ_FLAG_HIDDEN);
}

void page_camera_focus()
{
    lv_group_focus_obj(lv_camera);
    client_req_cb.req_finish = req_finish_callback; // http请求完成回调

    //设置初始化位置
    lv_obj_set_y(lv_info, -lv_obj_get_height(lv_info));
    // lv_obj_set_y(lv_qr_box, -lv_obj_get_height(lv_qr_box));

    func_sw_cb(m_bpack.sw_index);

    //开启视频显示任务
    controller_start_task(CF_VIDEO);
}

/******************handler*******************/
void camera_capture_handler(control_param_t *cpt)
{
    //转为jpg格式
    bool jpeg_converted = fmt2jpg(cpt->buf_t->buf,
                                  cpt->buf_t->buf_len,
                                  cpt->buf_t->buf_w,
                                  cpt->buf_t->buf_h,
                                  PIXFORMAT_RGB565, 80, &file_req.file_buf, &file_req.file_buf_len);
    if (!jpeg_converted)
    {
        ESP_LOGE(GUI_TAG, "JPEG compression failed");
        return;
    }
    //上传
    client_task_begin(TASK_FILE_UPLOAD);

    lv_label_set_text(lv_warn_label, UPLOAD_LABEL_TXT);
    lv_obj_clear_flag(lv_warn_label, LV_OBJ_FLAG_HIDDEN);
}

void camera_video_handler(control_param_t *cpt)
{
    // ESP_LOGI(GUI_TAG, "Picture: %d\n", cpt->buf_t->buf_len);

    jpeg_dsc.data_size = FRAME_SIZE;
    jpeg_dsc.data = (cpt->buf_t->buf);
    lv_img_set_src(lv_img_stream, &jpeg_dsc);
}

void camera_qrcode_handler(control_param_t *cpt)
{
    ESP_LOGI(GUI_TAG, "QRcode callback");
    lv_label_set_text(lv_warn_label, "");
    lv_obj_add_flag(lv_warn_label, LV_OBJ_FLAG_HIDDEN);

    //二维码扫描完成回调
    show_qr_result(cpt->qrcode);
    m_bpack.qr_flag = 1;
    //开启视频流
    controller_start_task(CF_VIDEO);
}

static void req_finish_callback(bool state)
{

    lv_label_set_text(lv_warn_label, "");
    lv_obj_add_flag(lv_warn_label, LV_OBJ_FLAG_HIDDEN);

    if (state)
        controller_sound_task();
    controller_start_task(CF_VIDEO);
}
