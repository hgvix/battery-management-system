#include "ds18b20.h"

#define DS18B20_PORT GPIOA
#define DS18B20_PIN GPIO_PIN_1

void Set_Pin_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

void Set_Pin_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

uint8_t DS18B20_Start(void)
{
    uint8_t response = 0;
    Set_Pin_Output();
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, 0);
    HAL_Delay(1);
    Set_Pin_Input();
    HAL_Delay(1);

    if (!(HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN)))
        response = 1;
    else
        response = 0;

    HAL_Delay(1);
    return response;
}

void DS18B20_Write(uint8_t data)
{
    Set_Pin_Output();

    for (int i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, 0);
        if ((data >> i) & 1)
        {
            HAL_Delay(1);
            HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, 1);
        }
        HAL_Delay(1);
    }
}

uint8_t DS18B20_Read(void)
{
    uint8_t value = 0;
    Set_Pin_Input();

    for (int i = 0; i < 8; i++)
    {
        if (HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN))
            value |= 1 << i;
        HAL_Delay(1);
    }
    return value;
}

float DS18B20_GetTemp(void)
{
    uint8_t temp_l, temp_h;
    int16_t temp;

    DS18B20_Start();
    DS18B20_Write(0xCC); // Skip ROM
    DS18B20_Write(0x44); // Convert T

    HAL_Delay(750);

    DS18B20_Start();
    DS18B20_Write(0xCC);
    DS18B20_Write(0xBE); // Read Scratchpad

    temp_l = DS18B20_Read();
    temp_h = DS18B20_Read();

    temp = (temp_h << 8) | temp_l;
    return (float)temp / 16.0;
}
