/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
  * it is free of charge, to any person obtaining a copy of this software and associated
  * documentation files (the "Software"), to deal in the Software without restriction, including
  * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all copies or
  * substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  */
#ifndef _QRCODE_RECOGINIZE_H_
#define _QRCODE_RECOGINIZE_H_

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "quirc.h"
#include "quirc_internal.h"

enum{
	RECONGIZE_OK,
	RECONGIZE_FAIL
};

#define PRINT_QR 0

// static char *TAG = "QR-recognizer";

static const char *data_type_str(int dt)
{
    switch (dt) {
    case QUIRC_DATA_TYPE_NUMERIC:
        return "NUMERIC";
    case QUIRC_DATA_TYPE_ALPHA:
        return "ALPHA";
    case QUIRC_DATA_TYPE_BYTE:
        return "BYTE";
    case QUIRC_DATA_TYPE_KANJI:
        return "KANJI";
    }
    return "unknown";
}

static void dump_cells(const struct quirc_code *code)
{
    int u = 0, v = 0;

    printf("    %d cells, corners:", code->size);
    for (u = 0; u < 4; u++) {
        printf(" (%d,%d)", code->corners[u].x, code->corners[u].y);
    }
    printf("\n");

    for (v = 0; v < code->size; v++) {
		printf("\033[0m    ");
        for (u = 0; u < code->size; u++) {
            int p = v * code->size + u;

            if (code->cell_bitmap[p >> 3] & (1 << (p & 7))) {
				printf("\033[40m  ");
            } else {
				printf("\033[47m  ");
            }
        }
		printf("\033[0m\n");
    }
}

static void dump_data(const struct quirc_data *data)
{
    printf("    Version: %d\n", data->version);
    printf("    ECC level: %c\n", "MLHQ"[data->ecc_level]);
    printf("    Mask: %d\n", data->mask);
    printf("    Data type: %d (%s)\n", data->data_type,
           data_type_str(data->data_type));
    printf("    Length: %d\n", data->payload_len);
    printf("\033[31m    Payload: %s\n", data->payload);

    if (data->eci) {
        printf("\033[31m    ECI: %d\n", data->eci);
    }
    printf("\033[0m\n");
}

static void dump_info(struct quirc *q, uint8_t count, char*pcode, int code_len)
{
    printf("%d QR-codes found:\n\n", count);
    count = 1; //只读一个二维码值，不考虑多个情况
    for (int i = 0; i < count; i++) {
        struct quirc_code code;
        struct quirc_data data;

        // Extract the QR-code specified by the given index.
        quirc_extract(q, i, &code);

        //Decode a QR-code, returning the payload data.
        quirc_decode_error_t err = quirc_decode(&code, &data);

#if PRINT_QR
        dump_cells(&code);
        printf("\n");
#endif

        if (err) {
            printf("  Decoding FAILED: %s\n", quirc_strerror(err));
        } else {
            printf("  Decoding successful:\n");
            printf("    %d cells, corners:", code.size);
            for (uint8_t u = 0; u < 4; u++) {
                printf(" (%d,%d)", code.corners[u].x, code.corners[u].y);
            }
            printf("\n");
            dump_data(&data);

            //长度限制128
            uint8_t m_len = data.payload_len;
            m_len = m_len > code_len ? code_len : m_len;
            memcpy(pcode, data.payload, data.payload_len);
        }
        printf("\n");
    }
}

#endif /* _QRCODE_RECOGINIZE_H_ */
