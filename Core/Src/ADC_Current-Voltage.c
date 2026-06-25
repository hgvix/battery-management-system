#include "ADC_Current-Voltage.h"

uint16_t ADC_GetAverage(uint8_t channel)
{
    uint32_t sum = 0;

    for(int i = 0; i < ADC_SAMPLES; i++)
    {
        sum += adcBufProcess[(i * ADC_CHANNELS) + channel];
    }

    return (sum / ADC_SAMPLES);
}

void ADC_UpdateVdda(void)
{
    uint16_t adcVref = ADC_GetAverage(7);
    g_vdda = 1.21f * 4095.0f / adcVref;
	printf("ADC ref: %d\r\n", adcVref);
	printf("V ref: %.2f V\r\n", g_vdda);
}


void ACS712_Calibrate(void)
{
    uint32_t sum = 0;

    for(int i = 0; i < 1000; i++)
    {
        sum += ADC_GetAverage(0);
        HAL_Delay(1);
    }

    float adcAverage = (float)sum / 1000.0;

    // ADC voltage
    float adcVoltage = (adcAverage * g_vdda) / 4095.0;

    // recover real ACS712 output voltage
    currentOffset = adcVoltage * 2.0;

}


float ACS712_ReadCurrent(uint8_t ADC_channel, float currentOffset)
{
    uint16_t adcCurrentRaw;
    float rawVoltage = 0.0;
    adcCurrentRaw = ADC_GetAverage(ADC_channel);
    // ADC voltage
    float adcVoltage = ((float)adcCurrentRaw * g_vdda) / 4095.0;

    // recover real ACS712 voltage
    rawVoltage = adcVoltage * 2.0;

    float current = (rawVoltage - currentOffset) / sensitivity;
printf("rawVoltage: %.6f", rawVoltage);
    // deadband chống nhiễu
    if(current > -0.0 && current < 0.05)
    {
        current = 0;
    }

    return current;
}

float Read_Voltage(uint8_t adcChannel)
{
	uint16_t adcRaw = ADC_GetAverage(adcChannel);
	float voltage = ((float)adcRaw * g_vdda / 4095.0f) * 5.7f;
	return voltage;
}

void Cell_ReadVoltage(float *cellVoltage, float *batteryVoltage)
{

	float pointVoltage[4];
    for(int i = 0; i < 4; i++)
    {
        pointVoltage[i] = Read_Voltage(i);
    }
    cellVoltage[0] = pointVoltage[0] - (0.1775*Current) + 0.013;
    cellVoltage[1] = pointVoltage[1] - pointVoltage[0] - (0.08*Current) + 0.088;
    cellVoltage[2] = pointVoltage[2] - pointVoltage[1] - (0.07*Current) - 0.01;
    cellVoltage[3] = pointVoltage[3] - pointVoltage[2] - (0.1578*Current) + 0.09;

    *batteryVoltage = cellVoltage[0] + cellVoltage[1] + cellVoltage[2] + cellVoltage[3];
}

void Battery_GetMinMaxCellVoltage(float *cellVoltage, float *minCell, float *maxCell)
{
    *minCell = cellVoltage[0];
    *maxCell = cellVoltage[0];

    for(uint8_t i = 1; i < 4; i++)
    {
        if(cellVoltage[i] > *maxCell)
        {
            *maxCell = cellVoltage[i];
        }

        if(cellVoltage[i] < *minCell)
        {
            *minCell = cellVoltage[i];
        }
    }
}
