/**
 * @file bms_protocol.c
 * @brief Binary UART protocol for STM32 → ESP32 BMS data transmission
 */

#include "bms_protocol.h"
#include "protection.h"
#include "balance.h"

/* -------------------------------------------------------------------------- */
/*  CRC-8 (polynomial 0x31, init 0xFF, no reflection, no final XOR)           */
/* -------------------------------------------------------------------------- */

static uint8_t crc8(const uint8_t *data, uint16_t len)
{
    uint8_t crc = 0xFF;
    while (len--)
    {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/* -------------------------------------------------------------------------- */
/*  Helper: build the fault bitmask from current protection state              */
/* -------------------------------------------------------------------------- */

static uint16_t build_fault_mask(float Temp, float current, float minCell, float maxCell)
{
    uint16_t faults = BMS_FAULT_NONE;

    if (Temp > OVER_TEMP_THRESHOLD)
        faults |= BMS_FAULT_OVER_TEMP;

    if (Temp < UNDER_TEMP_THRESHOLD)
        faults |= BMS_FAULT_UNDER_TEMP;

    if (current < -OVER_DISCHARGE_CURRENT_THRESHOLD)
        faults |= BMS_FAULT_OVER_DIS_I;

    if (current > OVER_CHARGE_CURRENT_THRESHOLD)
        faults |= BMS_FAULT_OVER_CHG_I;

    if (minCell < UNDER_BATT_VOLT_THRESHOLD)
        faults |= BMS_FAULT_UNDER_VOLT;

    if (maxCell > OVER_BATT_VOLT_THRESHOLD)
        faults |= BMS_FAULT_OVER_VOLT;

    return faults;
}

/* -------------------------------------------------------------------------- */
/*  Helper: derive a simple system-state byte                                  */
/* -------------------------------------------------------------------------- */

static uint8_t build_state(uint8_t chargeSignal, uint8_t chargeEnable, uint8_t dischargeEnable, uint16_t faults)
{
    if (faults != BMS_FAULT_NONE)
        return 3;   /* FAULT */

    if (chargeSignal == 0)
        return 1;   /* CHARGING (charger present) */

    if (dischargeEnable)
        return 2;   /* DISCHARGING */

    /* Check if any cell is balancing (idle + balancing active) */
    for (uint8_t i = 0; i < 4; i++)
    {
        if (Balance_GetState(i))
            return 4;   /* BALANCING */
    }

    return 0;       /* IDLE */
}

/* -------------------------------------------------------------------------- */
/*  Public: transmit one BMS packet                                           */
/* -------------------------------------------------------------------------- */

void BMS_Transmit(UART_HandleTypeDef *huart)
{
    uint8_t tx_buf[BMS_PACKET_SIZE];
    BMS_Payload_t payload;

    /* Fill payload ------------------------------------------------------- */
    memcpy(payload.cellVoltage,    cellVoltage,    4 * sizeof(float));
    memcpy(payload.cellSOC,        battery.cellSOC, 4 * sizeof(float));

    payload.batteryVoltage    = batteryVoltage;
    payload.current           = Current;
    payload.dischargeCurrent  = dischargeCurrent;
    payload.dischargeVoltage  = dischargeVoltage;
    payload.temperature       = Temp;
    payload.chargeSignal      = chargeSignal;
    payload.chargeEnable      = chargeEnable;
    payload.dischargeEnable   = dischargeEnable;
    payload.soc               = battery.soc;
    payload.soh               = battery.soh;
    payload.faults            = build_fault_mask(Temp, Current, minCell, maxCell);

    payload.state             = build_state(chargeSignal, chargeEnable, dischargeEnable, payload.faults);

    for (uint8_t i = 0; i < 4; i++)
        payload.balanceState[i] = Balance_GetState(i);

    memset(payload._reserved, 0, sizeof(payload._reserved));

    /* Build frame --------------------------------------------------------- */
    uint16_t offset = 0;

    tx_buf[offset++] = BMS_SYNC_BYTE;
    tx_buf[offset++] = BMS_PACKET_ID;
    tx_buf[offset++] = (uint8_t)(BMS_PAYLOAD_SIZE & 0xFF);        /* LEN_L */
    tx_buf[offset++] = (uint8_t)((BMS_PAYLOAD_SIZE >> 8) & 0xFF); /* LEN_H */

    memcpy(&tx_buf[offset], &payload, BMS_PAYLOAD_SIZE);
    offset += BMS_PAYLOAD_SIZE;

    tx_buf[offset] = crc8(tx_buf, offset);

    /* Transmit ------------------------------------------------------------ */
    HAL_UART_Transmit(huart, tx_buf, BMS_PACKET_SIZE, HAL_MAX_DELAY);
}
