#include "voltage.h"

static ADC_HandleTypeDef *adc_handle = NULL;
static volatile uint16_t adc_dma_buffer[VOLTAGE_CHANNEL_COUNT];
static volatile uint8_t adc_frame_ready = 0U;

/**
 * @brief  Initialize and start ADC in DMA mode for voltage sampling.
 *
 * @details
 * This function configures the ADC peripheral to operate with DMA and starts
 * continuous conversion. The ADC handle is stored internally and the frame
 * ready flag is reset before starting the conversion.
 *
 * @param[in] hadc  Pointer to the ADC handle. Must not be NULL.
 *
 * @pre   ADC peripheral must be properly initialized.
 * @post  ADC DMA conversion is running and filling the internal buffer.
 *
 */
HAL_StatusTypeDef Voltage_StartDma(ADC_HandleTypeDef *hadc)
{
    adc_handle = hadc;
    adc_frame_ready = 0U;

    return HAL_ADC_Start_DMA(adc_handle, (uint32_t *)adc_dma_buffer, VOLTAGE_CHANNEL_COUNT);
}


/**
 * @brief  Check if a complete ADC frame is available.
 *
 * @details
 * This function returns the status of the internal frame-ready flag,
 * which indicates that a full ADC DMA transfer cycle has completed.
 *
 * @param[in] None
 *
 * @return uint8_t
 *         - 1U: New ADC data frame is ready
 *         - 0U: No new data available
 *
 * @note  The flag is typically set in an interrupt context.
 */
uint8_t Voltage_IsFrameReady(void)
{
    return adc_frame_ready;
}


/**
 * @brief  Clear the ADC frame-ready flag.
 *
 * @details
 * This function resets the internal flag that indicates availability
 * of a new ADC data frame. It should be called after the data has been
 * processed.
 *
 * @param[in] None
 *
 * @return None
 *
 * @note  Ensure data processing is completed before calling this function.
 */
void Voltage_ClearFrameReady(void)
{
    adc_frame_ready = 0U;
}


/**
 * @brief  Convert raw ADC value to voltage.
 *
 * @details
 * This function reads the ADC value from the specified channel index
 * and converts it to a voltage using a fixed reference voltage (3.3V)
 * and ADC resolution (12-bit).
 *
 * @param[in] channel  ADC channel index.
 *
 * @return float
 *         - Valid voltage value (in volts) if channel is within range
 *         - 0.0F if channel index is invalid
 *
 * @pre   ADC buffer must contain valid conversion data.
 *
 * @note
 * - Assumes Vref = 3.3V and ADC resolution = 4095.
 * - No bounds checking beyond channel index validation.
 * - Unit: mV
 */
float Get_Voltage(uint8_t channel)
{
    uint16_t raw_value;

    if (channel >= VOLTAGE_CHANNEL_COUNT)
    {
        return 0.0f;
    }

    raw_value = adc_dma_buffer[channel];
    return (((float)raw_value * 3.3f) / 4095.0f)*1000;
}


/**
 * @brief  ADC conversion complete callback.
 *
 * @details
 * This function is called by the HAL layer when an ADC DMA transfer
 * is completed. It sets the internal frame-ready flag if the callback
 * corresponds to the configured ADC instance.
 *
 * @param[in] hadc  Pointer to the ADC handle triggering the callback.
 *
 * @return None
 *
 * @note
 * - Executed in interrupt context.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (adc_handle != NULL && hadc->Instance == adc_handle->Instance)	// HAL_ADC address == adc_handle (&hadc1)
    {
        adc_frame_ready = 1U;
    }
}


// void Balance_Voltage(uint8_t channel)
// {
//     if (channel >= VOLTAGE_CHANNEL_COUNT)
//     {
//         return 0.0f;
//     }


// }
