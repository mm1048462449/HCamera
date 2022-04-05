#include "gui_manager.h"

lv_style_t icon_style;
lv_style_t bg_white_style;
lv_obj_t* arc_loading;

lv_obj_t* pages[PAGE_COUNT]; //页面指针数组
page_focus_handler page_focus[PAGE_COUNT]; //页面焦点获取
page_init_handler page_inits[PAGE_COUNT]; //页面初始化

static void page_init(uint8_t index);
static void page_change_handler(void* arg);

static void page_init(uint8_t index)
{
    //初始化页面
    pages[index] = lv_obj_create(NULL);
    lv_obj_set_size(pages[index], GUI_WIDTH, GUI_HEIGHT);
    lv_obj_set_style_radius(pages[index], LV_RADIUS_CIRCLE, 0);
    lv_obj_center(pages[index]);

    page_inits[index](pages[index], page_change_handler);
}

static void page_change_handler(void* arg)
{
    uint8_t page_id = (uint8_t)arg;
    printf("page handler: %d\n", page_id);

    if (page_id != PAGE_MAIN)
    {
        page_init(page_id);
        lv_scr_load_anim(pages[page_id], LV_SCR_LOAD_ANIM_MOVE_LEFT, ANIM_TIME, 50, false);
    }
    else
    {
        lv_scr_load_anim(pages[page_id], LV_SCR_LOAD_ANIM_MOVE_RIGHT, ANIM_TIME, 50, true);
    }
       
    page_focus[page_id]();
}

static void gui_manager_init()
{
    //初始化样式
    init_background_style(&bg_white_style, LV_OPA_80, lv_color_white());
    init_font_style(&icon_style, lv_palette_lighten(LV_PALETTE_RED, 1), &hcamera_fonts_18);

    page_inits[PAGE_MAIN] = page_main_init;
    page_inits[PAGE_CAMERA] = page_camera_init;
    page_inits[PAGE_PICTURE] = page_picture_init;
    page_inits[PAGE_GAME] = page_game_init;
    page_inits[PAGE_SETTING] = page_setting_init;
    page_inits[PAGE_ABOUT] = page_about_init;

    page_focus[PAGE_MAIN] = page_main_focus;
    page_focus[PAGE_CAMERA] = page_camera_focus;
    page_focus[PAGE_PICTURE] = page_picture_focus;
    page_focus[PAGE_GAME] = page_game_focus;
    page_focus[PAGE_SETTING] = page_setting_focus;
    page_focus[PAGE_ABOUT] = page_about_focus;

    page_init(PAGE_MAIN);
    lv_scr_load(pages[PAGE_MAIN]);
    page_focus[PAGE_MAIN]();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++
esp_timer_handle_t gui_timer_handle = 0;

void lvgl_init()
{
    lv_init();            // lvgl 系统初始化
    lv_port_disp_init();  // lvgl 显示接口初始化
    lv_port_indev_init(); // lvgl 输入接口初始化，此处仅使用按键
}

void gui_tick_task(void *parameter)
{
    while (1)
    {
        lv_tick_inc(10);
        vTaskDelay(10 / portTICK_RATE_MS );
    }
    vTaskDelete(NULL);
}

void gui_timer_cb(void *arg)
{
    lv_task_handler();
}

void app_gui_main()
{
    lvgl_init();
    gui_manager_init();
    // return;

    xTaskCreate(&gui_tick_task, "GUI_TASK", 4096, NULL, 4, NULL);

    // 创建一个定时器
    esp_timer_create_args_t gui_timer ={
            .callback = &gui_timer_cb, //回调函数
            .arg = NULL,               //参数
            .name = "GUI_TIMER"        //定时器名称
        };

    esp_err_t err = esp_timer_create(&gui_timer, &gui_timer_handle);
    err = esp_timer_start_periodic(gui_timer_handle, 20 * 1000); 

    if (err == ESP_OK)
    {
        printf("gui timer create ok!\r\n");
    }
}

void app_gui_exit()
{
    if (gui_timer_handle != NULL)
    {
        esp_timer_stop(gui_timer_handle);   //定时器暂停
        esp_timer_delete(gui_timer_handle); //定时器删除
    }
}
