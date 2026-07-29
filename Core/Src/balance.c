#include "balance.h"

static uint8_t balanceState[4] = {0};

void Balance_Init(void)
{
    for(uint8_t i = 0; i < 4; i++)
    {
        balanceState[i] = 0;
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);
}

static void Balance_Output(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, balanceState[0] ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, balanceState[1] ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, balanceState[2] ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, balanceState[3] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Balance_Control(float *cellVoltage, float temperature, float minCell, float maxCell, uint8_t chargeSignal)
{
    float delta = maxCell - minCell;


    /* Không cân bằng khi:
       - Không sạc
       - Nhiệt độ quá cao
    */

    if(chargeSignal || temperature > BALANCE_MAX_TEMP)
    {
        for(uint8_t i = 0; i < 4; i++)
        {
            balanceState[i] = 0;
        }

        Balance_Output();
        return;
    }

    /* Điều kiện bật cân bằng */

    if((maxCell > BALANCE_START_VOLTAGE) && (delta > BALANCE_DELTA_START))
    {
        for(uint8_t i = 0; i < 4; i++)
        {
            if((cellVoltage[i] > (minCell + BALANCE_DELTA_START)) && cellVoltage[i] > BALANCE_START_VOLTAGE)
            {
                balanceState[i] = 1;
            }
            else
            {
            	 balanceState[i] = 0;
            }
        }
    }

    /* Điều kiện tắt cân bằng */

    if((maxCell < BALANCE_START_VOLTAGE) || (delta < BALANCE_STOP_DELTA))
    {
        for(uint8_t i = 0; i < 4; i++)
        {
            balanceState[i] = 0;
        }
    }
    Balance_Output();

	printf("BAL: %d %d %d %d\r\n",
	       Balance_GetState(0),
	       Balance_GetState(1),
	       Balance_GetState(2),
	       Balance_GetState(3));
}

uint8_t Balance_GetState(uint8_t cell)
{
    if(cell >= 4)
        return 0;

    return balanceState[cell];
}

