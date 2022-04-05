#include "app_client.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/**
 * response
 *
    {
        "pageNum": 1,
        "pageItem": 6,
        "pageCount": 1,
        "dir": "/HCamera",
        "files": [{
            "name": "bingdundun.png",
            "size": 50748,
            "date": "2022-01-01 00:00:00",
            "isDir": false
        }, {
            "name": "ss1.png",
            "size": 98429,
            "date": "2022-01-01 00:00:00",
            "isDir": false
        }]
    }
 *
 */
#define MAX_HTTP_OUTPUT_BUFFER 2048
#define POST_HEAD_TEXT_BEGIN "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"data\"; filename=\"/HCamera/"
#define POST_HEAD_TEXT_END "\"\r\nContent-Type: image/jpeg\r\n\r\n"
#define POST_TAIL_TEXT "\r\n--RandomNerdTutorials--\r\n"
#define POST_HEADER_CONTENT_TYPE "multipart/form-data; boundary=RandomNerdTutorials"

static const char *TAG = "app client";

file_req_t file_req = {
    .url = "http://192.168.4.1",
    .dir = "/HCamera/",
    .pn = 1,
    .pi = 6,
    .task_type = TASK_FILE_LIST,
};
req_cb_t client_req_cb;
file_res_t file_res;

void client_request(void);
void on_finish_handler(const char *data, int len);

//如果是在一个程序的循环中不断调用这个函数，那么是没有效果的
//虽然也是使用的系统的时间函数来初始化随机数发生器，但程序的
//执行速度太快了，可能执行1000次循环返回的秒数都是一样的
// time返回时间戳
static void get_rand_str(char s[], int num)
{
    //定义随机生成字符串表
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i, lstr;
    char ss[2] = {0};
    lstr = strlen(str);                        //计算字符串长度
    srand((unsigned int)time((time_t *)NULL)); //使用系统时间来初始化随机数发生器
    for (i = 1; i <= num; i++)
    {                                            //按指定大小返回相应的字符串
        sprintf(ss, "%c", str[(rand() % lstr)]); // rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
        strcat(s, ss);                           //将随机生成的字符串连接到指定数组后面
    }
}

static void parseJson(char *jsonstr)
{
    cJSON *root = NULL;
    root = cJSON_Parse(jsonstr);
    if (!root)
    {
        ESP_LOGE(TAG, "Error before: [%s]\n", cJSON_GetErrorPtr());
    }
    else
    {
        cJSON *item = NULL;
        //解析json数据
        item = cJSON_GetObjectItem(root, "pageNum");
        file_res.page_num = item->valueint;
        ESP_LOGI(TAG, "pageNum: %d", file_res.page_num);
        item = cJSON_GetObjectItem(root, "pageItem");
        file_res.page_item = item->valueint;
        ESP_LOGI(TAG, "pageItem: %d", file_res.page_item);
        item = cJSON_GetObjectItem(root, "pageCount");
        file_res.page_count = item->valueint;
        ESP_LOGI(TAG, "pageCount: %d", file_res.page_count);
        item = cJSON_GetObjectItem(root, "dir");
        file_res.dir = item->valuestring;
        ESP_LOGI(TAG, "dir: %s", file_res.dir);

        if (!client_req_cb.list_info)
        {
            ESP_LOGE(TAG, "list info callback is null");
            return;
        }
        client_req_cb.list_info();

        //解析文件数组
        item = cJSON_GetObjectItem(root, "files");
        if (!item)
        {
            ESP_LOGI(TAG, "no file list");
        }
        else
        {
            int list_count = cJSON_GetArraySize(item);
            cJSON *file;
            cJSON *file_item;
            for (int i = 0; i < list_count; i++)
            {
                file = cJSON_GetArrayItem(item, i);
                file_item = cJSON_GetObjectItem(file, "isDir");
                if (!file_item->valueint)
                {
                    file_item = cJSON_GetObjectItem(file, "name");

                    if (!client_req_cb.list_txt)
                    {
                        ESP_LOGE(TAG, "list add callback is null");
                        return;
                    }
                    client_req_cb.list_txt(i, (char *)file_item->valuestring); //更新列表显示
                }
            }
        }
    }

    if (root)
        cJSON_Delete(root);
}

void on_finish_handler(const char *data, int len)
{
    switch (file_req.task_type)
    {
    case TASK_FILE_LIST:
        parseJson(data);
        break;
    case TASK_FILE_UPLOAD:
        break;
    case TASK_FILE_DOWNLOAD:
        file_res.file_buf = data;
        file_res.file_len = len;
        break;
    }
}

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    static int output_all_len;

    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        if (client_req_cb.req_finish)
        {
            client_req_cb.req_finish(false);
        }
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
        printf("%.*s", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            if (output_buffer == NULL)
            {
                output_all_len = esp_http_client_get_content_length(evt->client);
                output_buffer = (char *)malloc(output_all_len);
                output_len = 0;
                if (output_buffer == NULL)
                {
                    ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                    return ESP_FAIL;
                }
            }
            memcpy(output_buffer + output_len, evt->data, evt->data_len);
            output_len += evt->data_len;

            if (client_req_cb.req_prog && file_req.task_type == TASK_FILE_DOWNLOAD)
            {
                float perc = ((output_len * 1.0f / output_all_len) * 100);
                client_req_cb.req_prog(perc);
            }
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        on_finish_handler(output_buffer, output_len);
        if (client_req_cb.req_finish)
        {
            client_req_cb.req_finish(true);
        }
        if (output_buffer != NULL)
        {
            // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        output_all_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

int client_check_connected()
{
    return wifi_connected;
}

static void _filelist_request()
{
    // char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    // int content_length = 0;

    esp_http_client_config_t config = {
        .url = file_req.url,
        .event_handler = _http_event_handle};

    esp_http_client_handle_t client = esp_http_client_init(&config);
    char url[64];
    memset(url, 0, sizeof(url));
    sprintf(url, "%s/list?dir=%s&pn=%d&pi=%d", file_req.url, file_req.dir, file_req.pn, file_req.pi);
    ESP_LOGI(TAG, "files api: %s", url);
    esp_http_client_set_url(client, url);
    esp_http_client_set_method(client, HTTP_METHOD_GET);

    esp_err_t err = esp_http_client_perform(client);
    // esp_err_t err = esp_http_client_open(client, 0);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));

        // content_length = esp_http_client_fetch_headers(client);
        // int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
        // ESP_LOGI(TAG, "%d %d %s", content_length, data_read, output_buffer);
        // parseJson(output_buffer);
    }

    // esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

static void _download_request()
{
    char url_str[128];
    sprintf(url_str, "%s%s%s?download", file_req.url, file_req.dir, file_req.file);
    ESP_LOGI(TAG, "%s", url_str);

    esp_http_client_config_t config = {
        .url = url_str,
        .event_handler = _http_event_handle,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }

    esp_http_client_cleanup(client);
}

static void _upload_request()
{
    if (!file_req.file_buf)
        return;

    char *post = (char *)malloc(1024 + file_req.file_buf_len);
    memset(post, 0, 1024 + file_req.file_buf_len);
    char url_str[64];
    sprintf(url_str, "%s/edit", file_req.url);
    ESP_LOGI(TAG, "%s", url_str);
    esp_http_client_config_t config = {
        .url = url_str,
        .event_handler = _http_event_handle,
        .method = HTTP_METHOD_POST,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // struct tm *t;
    // time_t tt;
    // time(&tt);
    // t = localtime(&tt);
    // //%4d年%02d月%02d日 %02d:%02d:%02d
    // char t_name[19];
    // ESP_LOGI(TAG, "%4d%02d%02d%02d%02d%02d.jpg", t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

    char file_name[21]; //文件名长度20
    char temp[16];
    get_rand_str(temp, 16);
    sprintf(file_name, "%s.jpg", temp);
    ESP_LOGI(TAG, "%s", file_name);

    int post_head_len = strlen(POST_HEAD_TEXT_BEGIN) + strlen(POST_HEAD_TEXT_END) + strlen(file_name);
    int post_tail_len = strlen(POST_TAIL_TEXT);
    char *post_head_text = (char *)malloc(post_head_len);
    sprintf(post_head_text, "%s%s%s", POST_HEAD_TEXT_BEGIN, file_name, POST_HEAD_TEXT_END);

    ESP_LOGI(TAG, "%s,%d", post_head_text, post_head_len);
    esp_http_client_set_header(client, "Content-Type", POST_HEADER_CONTENT_TYPE);
    memcpy(post, post_head_text, post_head_len);
    memcpy(post + post_head_len, file_req.file_buf, file_req.file_buf_len);
    memcpy(post + post_head_len + file_req.file_buf_len, POST_TAIL_TEXT, post_tail_len);

    esp_http_client_set_post_field(client, post, post_head_len + post_tail_len + file_req.file_buf_len);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);

    if (file_req.file_buf)
    {
        free(file_req.file_buf);
        file_req.file_buf = NULL;
        file_req.file_buf_len = 0;
    }
}

static void client_res_task()
{
    switch (file_req.task_type)
    {
    case TASK_FILE_LIST:
        _filelist_request();
        break;
    case TASK_FILE_UPLOAD:
        _upload_request();
        break;
    case TASK_FILE_DOWNLOAD:
        _download_request();
        break;
    }

    vTaskDelete(NULL);
}

void client_task_begin(int type)
{
    if (!app_config.wifi_state)
        return;
    file_req.task_type = type;
    xTaskCreate(&client_res_task, "HTTP RES TASK", 1024 * 16, NULL, 5, NULL);
}
