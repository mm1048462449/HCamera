#include "dino.h"

#define DINO_W lv_game_w
#define DINO_H lv_game_h

#define CACTUS_CNT 3    //障碍物类型
#define CACTUS_SPEED_MAX 5  //障碍物最大速度
#define CACTUS_MAX 2    //障碍物同屏最大数量
#define BERD_SPEED 3    //鸟速度
#define DINO_UP_SPEED 8     //Dino上升速度
#define DINO_DOWN_SPEED 4   //Dino下降速度
#define DINO_DOWN_WAIT 6    //Dino空中停留时间

#define RANDOM(A,B) rand()%(B - A + 1) + A

static void(*exit_cb)(void* arg);
static void obj_cb(uint8_t type);
static void draw_gameover();

typedef struct MoveObj {
    short x; //x
    short pos_x; //x坐标
    uint8_t y; //y坐标
    uint8_t hide_wait; //隐藏后显示等待时间
    uint8_t speed;
    uint8_t action_gap:5; //动作切换间隔
    uint8_t user_data : 2; //用户数据
    uint8_t is_alive : 1; //标志
    void(*obj_hide_cb)(uint8_t); //Obj隐藏后回调
}move_obj_t;

typedef struct DinoObj {
    uint16_t score; //分数
    uint8_t run_flag : 1; //运行标志
    uint8_t is_alive : 1; //标志
    uint8_t jump_flag : 1; //跳跃标志
    uint8_t x : 5; //x位置
    uint8_t jump_dir : 1; //跳跃方向
    uint8_t action_gap; //动作切换间隔
    uint8_t down_wait; //跳跃后滞留时间
    uint8_t pos_y; //y高度
    short h; //跳跃高度
}dino_obj_t;

dino_obj_t dino_obj = {
    .score = 0,
    .run_flag = 0,
    .is_alive = 1,
    .h = 0,
    .x = 10,
    .action_gap = 6,
    .jump_flag = 0,
    .down_wait = 0,
};

move_obj_t berd_obj = {
    .x = 0,
    .y = 0,
    .pos_x = 0,
    .hide_wait = 0,
    .action_gap = 8,
    .obj_hide_cb = obj_cb,
};

move_obj_t cactus_obj[CACTUS_CNT]; //仙人掌(大，中，小)

static uint8_t calc_obj_posx(short pos, uint8_t w)
{
    //return pos < 0 ? (w + pos) : pos;
    return pos < 0 ? 0 : pos;
}

static void obj_cb(uint8_t type)
{
    if (type == 3)
    {
        berd_obj.x = 0;
        berd_obj.y = 0; //获取范围内随机高度
        berd_obj.hide_wait = RANDOM(20, 100); //随机获取等待时间
        LV_LOG_USER("Hide t: %d", berd_obj.hide_wait);
    }
    else
    {

        //重置
        cactus_obj[type].x = 0;
        cactus_obj[type].is_alive = 0;
    }

}

static void draw_dino_obj(uint8_t cnt)
{
    static uint8_t n = 0;
    uint8_t jump_max = 70;
    if (dino_obj.jump_flag)
    {
        //清除旧图像
        lv_game_clear_rect(dino_obj.x, dino_obj.pos_y, dino_jump.header.w, dino_jump.header.h);

        //跳跃时动作
        if (dino_obj.jump_dir == 0)
        {
            //跳跃
            if (dino_obj.h < jump_max)
            {
                dino_obj.h += DINO_UP_SPEED;
            }
            else
            {
                dino_obj.h = jump_max;
                dino_obj.down_wait = DINO_DOWN_WAIT;
                dino_obj.jump_dir = 1;
            }
        }
        else
        {
            //空中滞留
            if (dino_obj.down_wait > 0)
            {
                dino_obj.down_wait--;
                return;
            }
            //下降
            if (dino_obj.h >= 0)
            {
                dino_obj.h -= DINO_DOWN_SPEED;
            }
            else
            {
                dino_obj.h = 0;
                dino_obj.jump_dir = 0;
                dino_obj.jump_flag = 0;

                //跳跃完成，分数加一//不太准确，没有考虑非障碍物跳跃情况...
                dino_obj.score++;
                LV_LOG_USER("Score: %d", dino_obj.score);
            }
        }

        //计算y轴坐标
        dino_obj.pos_y = DINO_H - (dino_jump.header.h) - dino_obj.h;

        lv_game_drawimg(dino_obj.x, dino_obj.pos_y, &dino_jump);
        return;
    }
    else if (cnt % dino_obj.action_gap == 0)
    {
        //非跳跃时动作
        n++;
        if (n >= 2) n = 0;

        lv_game_clear_rect(dino_obj.x, DINO_H - (dino_run_0.header.h) - dino_obj.h, dino_run_0.header.w, dino_run_0.header.h);
        switch (n)
        {
        case 0:
            lv_game_drawimg(dino_obj.x, DINO_H - (dino_run_0.header.h) - dino_obj.h, &dino_run_0);
            break;
        case 1:
            lv_game_drawimg(dino_obj.x, DINO_H - (dino_run_1.header.h) - dino_obj.h, &dino_run_1);
            break;
        }
    }
}

static void cactus_obj_move(uint8_t n)
{
    short cactus_w = 0;

    //隐藏时间
    if (cactus_obj[n].hide_wait > 0)
    {
        cactus_obj[n].hide_wait--;
        return;
    }

    switch (n)
    {
    case 0:
        cactus_w = cactus_big.header.w;
        if (cactus_obj[n].is_alive)
            lv_game_clear_rect(calc_obj_posx(cactus_obj[n].pos_x, cactus_w), cactus_obj[n].y, cactus_big.header.w, cactus_big.header.h);
        break;
    case 1:
        cactus_w = cactus_small.header.w;
        if (cactus_obj[n].is_alive)
            lv_game_clear_rect(calc_obj_posx(cactus_obj[n].pos_x, cactus_w), cactus_obj[n].y, cactus_small.header.w, cactus_small.header.h);
        break;
    case 2:
        cactus_w = cactus_small_many.header.w;
        if (cactus_obj[n].is_alive)
            lv_game_clear_rect(calc_obj_posx(cactus_obj[n].pos_x, cactus_w), cactus_obj[n].y, cactus_small_many.header.w, cactus_small_many.header.h);
        break;
    }

    if (cactus_obj[n].pos_x <= -(cactus_w))
    {
        LV_LOG_USER("Cactus %d hide", n);

        cactus_obj[n].pos_x = 0;
        cactus_obj[n].obj_hide_cb(n);
        return;
    }

    if (cactus_obj[n].is_alive)
    {
        cactus_obj[n].x += cactus_obj[n].speed;
        cactus_obj[n].pos_x = DINO_W - cactus_obj[n].x;

        switch (n)
        {
        case 0:
            lv_game_drawimg(cactus_obj[n].pos_x, cactus_obj[n].y, &cactus_big);
            break;
        case 1:
            lv_game_drawimg(cactus_obj[n].pos_x, cactus_obj[n].y, &cactus_small);
            break;
        case 2:
            lv_game_drawimg(cactus_obj[n].pos_x, cactus_obj[n].y, &cactus_small_many);
            break;
        }
    }

}

static void cactus_obj_detect(uint8_t cnt)
{
    //开始计算
    static uint8_t count = 0;
    uint8_t cactus_num = 0;
    uint8_t cactus_type = 0;
    for (int i = 0; i < CACTUS_CNT; i++)
    {
        if (cactus_obj[i].is_alive && cactus_obj[i].x > 0)
        {
            cactus_num++;
        }
    }

    if (cactus_num >= CACTUS_MAX)
    {
        return;
    }

    if (count > RANDOM(45, 85) && (count % RANDOM(0,5) == 0))
    {
        count = 0;
        cactus_type = RANDOM(0, 2);
        if (!cactus_obj[cactus_type].is_alive)
        {
            cactus_obj[cactus_type].is_alive = 1;
        }
    }

    count++;
    if (count >= 255) count = 0;
}

static void draw_cactus_obj(uint8_t cnt)
{
    for (int i = 0; i < CACTUS_CNT; i++)
    {
        if (cactus_obj[i].is_alive && (cnt % cactus_obj[i].action_gap == 0))
        {
            cactus_obj_move(i);
        }
    }

}

static void draw_berd_obj(uint8_t cnt)
{
    static uint8_t n = 0;
    short berd_w = berd.header.w;

    if (cnt % berd_obj.action_gap == 0)
    {
        //隐藏时间
        if (berd_obj.hide_wait > 0)
        {
            berd_obj.hide_wait--;
            return;
        }

        //清除旧图像
        lv_game_clear_rect(calc_obj_posx(berd_obj.pos_x, berd_w), berd_obj.y, berd.header.w, berd.header.h);

        if (berd_obj.pos_x <= -(berd_w))
        {
            LV_LOG_USER("Berd hide");

            berd_obj.pos_x = 0;
            berd_obj.obj_hide_cb(3);
            return;
        }

        berd_obj.x+=BERD_SPEED;
        berd_obj.pos_x = DINO_W - berd_obj.x;

        //非跳跃时动作
        n++;
        if (n >= 2) n = 0;
        switch (n)
        {
        case 0:
            lv_game_drawimg(berd_obj.pos_x, berd_obj.y, &berd);
            break;
        case 1:
            lv_game_drawimg(berd_obj.pos_x, berd_obj.y, &berd1);
            break;
        }
    }

}

static bool detect_collision()
{
    uint8_t dino_border_left = dino_obj.x;
    uint8_t dino_border_right = dino_obj.x + dino_jump.header.w;
    short dino_border_bottom = LV_MAX(dino_obj.h, 0);

    for (int i = 0; i < CACTUS_CNT; i++)
    {
        uint8_t cactus_h = 0;
        uint8_t cactus_w = 0;
        switch (i)
        {
        case 0:
            cactus_h = cactus_big.header.h;
            cactus_w = cactus_big.header.w;
            break;
        case 1:
            cactus_h = cactus_small.header.h;
            cactus_w = cactus_small.header.w;
            break;
        case 2:
            cactus_h = cactus_small_many.header.h;
            cactus_w = cactus_small_many.header.w;
            break;
        }

        if (!cactus_obj[i].is_alive)continue;

        if (dino_border_bottom <= cactus_h)
        {
            uint8_t cactus_left = DINO_W - cactus_obj[i].x;
            uint8_t cactus_right = (DINO_W - cactus_obj[i].x + cactus_w);
            bool b1 = dino_border_left > cactus_left && dino_border_left <= cactus_right;
            bool b2 = dino_border_right >= cactus_left && dino_border_right < cactus_right;

            if (b1 || b2)
            {
                draw_gameover();
                break;
            }
        }
    }

    return true;
}

static void dino_update_cb(lv_timer_t* timer)
{

    static uint8_t run_cnt = 0;
    if (!dino_obj.run_flag) return;

    //Berd绘制
    draw_berd_obj(run_cnt);

    //Dino绘制
    draw_dino_obj(run_cnt);

    //Cactus绘制
    draw_cactus_obj(run_cnt);
    cactus_obj_detect(run_cnt);

    //计算是否碰撞
    detect_collision();

    run_cnt++;
    if (run_cnt >= 255) run_cnt = 0;
}

static void key_left_cb()
{
    //返回
    dino_exit();
}

static void key_right_cb()
{
    //开始
    if (!dino_obj.run_flag)
    {
        lv_game_clear(lv_color_white());

        dino_obj.run_flag = 1;
        dino_obj.jump_flag = 0;
        dino_obj.is_alive = 1;
        dino_obj.score = 0;
        dino_obj.pos_y = 0;
        dino_obj.h = 0;
        dino_obj.jump_dir = 0;

        berd_obj.x = 0;
        berd_obj.y = 0;
        berd_obj.hide_wait = 0;
        berd_obj.pos_x = 0;

        for (int i = 0; i < CACTUS_CNT; i++)
        {
            cactus_obj[i].x = 0;
            cactus_obj[i].y = 0;
            cactus_obj[i].hide_wait = 0;
            cactus_obj[i].speed = 5;
            cactus_obj[i].pos_x = 0;
            cactus_obj[i].user_data = i;
            cactus_obj[i].action_gap = 1;
            cactus_obj[i].obj_hide_cb = obj_cb;
            cactus_obj[i].is_alive = 0;
        }
        cactus_obj[0].y = DINO_H - cactus_big.header.h;
        cactus_obj[1].y = DINO_H - cactus_small.header.h;
        cactus_obj[2].y = DINO_H - cactus_small_many.header.h;
        cactus_obj[0].is_alive = 1;
    }
    else
    {
        LV_LOG_USER("Dino running...");
    }
}

static void key_enter_cb()
{
    //执行跳跃动作
    dino_obj.jump_flag = 1;
}

static void draw_logo()
{
    lv_game_clear(lv_color_white());

    int pos_x = 10;
    int pos_y = DINO_H - cactus_big.header.h;

    lv_game_drawimg(pos_x, pos_y + (cactus_big.header.h - dino_logo.header.h), &dino_logo);
    lv_game_drawimg((DINO_W - cactus_big.header.w) >> 1, pos_y, &cactus_big);

    lv_game_drawtxt(35, 25, LV_SIZE_CONTENT, "Press right start");
}

static void draw_gameover()
{
    int pos_x = 50;
    int pos_y = 25;
    char score_str[32];
    dino_obj.run_flag = 0;
    sprintf(score_str, "Score: %d", dino_obj.score);

    lv_game_drawtxt(pos_x, pos_y, LV_SIZE_CONTENT, "Game Over");
    lv_game_drawtxt(pos_x, pos_y + 20, LV_SIZE_CONTENT, score_str);
}

void dino_init(lv_obj_t* canvas, void(*exit_handle)(void* arg))
{
    lv_game_set_canvas(canvas);
    exit_cb = exit_handle;

}

void dino_run()
{
    lv_game_reg_keyleft(key_left_cb);
    lv_game_reg_keyright(key_right_cb);
    lv_game_reg_keyenter(key_enter_cb);
    lv_game_reg_update(dino_update_cb);
    lv_game_begin(100);

    dino_obj.run_flag = 0;
    draw_logo();
}

void dino_exit()
{
    int exit_flag = 0;
    lv_game_end();
    exit_cb(&exit_flag);
}
