#include "pcf8575.h"
#include <driver/i2c.h>
#include "twi.h"

#define I2C_MASTER_SDA_IO 3 /*!< gpio number for I2C master data  */
#define I2C_MASTER_SCL_IO 15 /*!< gpio number for I2C master clock */

#define I2C_MASTER_NUM 1            /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */
#define I2C_MASTER_TIMEOUT_MS 1000

#define PCF8575_BASE_ADDR 0x20

static const char *TAG = "pcf8575";

uint16_t writeMode = 0;
uint16_t readMode = 0;
uint16_t byteBuffered = 0;
uint16_t writeByteBuffered = 0;

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 @brief I2C写数据函数
 @param slaveAddr -[in] 从设备地址
 @param regAddr -[in] 寄存器地址
 @param pData -[in] 写入数据
 @param dataLen -[in] 写入数据长度
 @return 错误码
*/
static esp_err_t i2c_write_data(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen)
{
    int ret = ESP_OK;
    uint8_t write_buf[2] = {*pData, *(pData + 1)};
    ret = i2c_master_write_to_device(I2C_MASTER_NUM,
                                     slaveAddr,
                                     write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);

    return ret;
}

/**
 @brief I2C读数据函数
 @param slaveAddr -[in] 从设备地址
 @param regAddr -[in] 寄存器地址
 @param pData -[in] 读出数据
 @param dataLen -[in] 读出数据长度
 @return 错误码
*/
static esp_err_t i2c_read_data(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen)
{
    return i2c_master_read_from_device(I2C_MASTER_NUM,
                                       slaveAddr,
                                       pData,
                                       dataLen,
                                       I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
}

void pcf8575_begin(void)
{
    ESP_LOGI(TAG, "pcf8575 init...");
    ESP_ERROR_CHECK(i2c_master_init());
}

void pcf8575_pinmode(uint8_t pin, uint8_t mode)
{
    if (mode == PCF_OUTPUT)
    {
        writeMode = writeMode | bit(pin);
        readMode = readMode & ~bit(pin);
    }
    else if (mode == PCF_INPUT)
    {
        writeMode = writeMode & ~bit(pin);
        readMode = readMode | bit(pin);
    }
    else
    {
        ESP_LOGE(TAG, "Mode non supported by PCF8575");
    }
}

void pcf8575_write(uint8_t pin, uint8_t value)
{
    if (value == PCF_HIGH)
    {
        writeByteBuffered = writeByteBuffered | bit(pin);
    }
    else
    {
        writeByteBuffered = writeByteBuffered & ~bit(pin);
    }
    writeByteBuffered = writeByteBuffered & writeMode;
    uint8_t temp[2];
    temp[1] = writeByteBuffered >> 8;
    temp[0] = writeByteBuffered & 0xff;
    i2c_write_data(PCF8575_BASE_ADDR, NULL, &writeByteBuffered, 2);
}

uint8_t pcf8575_read(uint8_t pin)
{
    uint8_t value = PCF_LOW;
    if ((bit(pin) & writeMode) > 0)
    {
        if ((bit(pin) & writeByteBuffered) > 0)
        {
            value = PCF_HIGH;
        }
        else
        {
            value = PCF_LOW;
        }
        return value;
    }

    if ((bit(pin) & byteBuffered) > 0)
    {
        value = PCF_HIGH;
    }
    else if ((bit(pin) & byteBuffered) <= 0)
    {
        uint16_t iInput = 0;
        i2c_read_data(PCF8575_BASE_ADDR, NULL, &iInput, 2);

        if ((iInput & readMode) > 0)
        {
            byteBuffered = byteBuffered | (uint16_t)iInput;

            if ((bit(pin) & byteBuffered) > 0)
            {
                value = PCF_HIGH;
            }
        }
    }

    if (value == PCF_HIGH)
    {
        byteBuffered = ~bit(pin) & byteBuffered;
    }

    return value;
}
