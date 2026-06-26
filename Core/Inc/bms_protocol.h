/**
 * @file bms_protocol.h
 * @brief Binary UART protocol for STM32 → ESP32 BMS data transmission
 *
 * Protocol: [0xAA][PACKET_ID][LEN_L][LEN_H][PAYLOAD...][CRC8]
 * Baudrate: 115200
 * Interval: 2 seconds (every main loop cycle)
 */

#ifndef INC_BMS_PROTOCOL_H_
#define INC_BMS_PROTOCOL_H_

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "protection.h"
#include "battery.h"

/* -------------------------------------------------------------------------- */
/*  Packet constants                                                          */
/* -------------------------------------------------------------------------- */

#define BMS_SYNC_BYTE      0xAA
#define BMS_PACKET_ID      0x01
#define BMS_PAYLOAD_SIZE   80
#define BMS_PACKET_SIZE    (1 + 1 + 2 + BMS_PAYLOAD_SIZE + 1)  /* sync + id + len + payload + crc */

/* -------------------------------------------------------------------------- */
/*  Fault bitmask (mirrored on ESP32)                                         */
/* -------------------------------------------------------------------------- */

#define BMS_FAULT_NONE             0x0000
#define BMS_FAULT_OVER_TEMP        (1U << 0)   /* Temperature > 60 C              */
#define BMS_FAULT_UNDER_TEMP       (1U << 1)   /* Temperature < 0 C               */
#define BMS_FAULT_OVER_DIS_I       (1U << 2)   /* Discharge current > 3.0 A       */
#define BMS_FAULT_OVER_CHG_I       (1U << 3)   /* Charge current > 2.6 A          */
#define BMS_FAULT_UNDER_VOLT       (1U << 4)   /* Cell voltage < 3.2 V            */
#define BMS_FAULT_OVER_VOLT        (1U << 5)   /* Cell voltage > 4.2 V            */

/* -------------------------------------------------------------------------- */
/*  External globals (defined in main.c / battery.c / protection.c)             */
/* -------------------------------------------------------------------------- */

extern float  cellVoltage[4];
extern float  batteryVoltage;
extern float  dischargeVoltage;
extern float  minCell;
extern float  maxCell;
extern float  Current;
extern float  dischargeCurrent;
extern float  Temp;
extern uint8_t chargeSignal;
extern uint8_t chargeEnable;
extern uint8_t dischargeEnable;

/* -------------------------------------------------------------------------- */
/*  Payload layout (must match ESP32 side exactly)                             */
/* -------------------------------------------------------------------------- */

typedef struct __attribute__((packed))
{
    /* Cell voltages (V) */
    float cellVoltage[4];          /* offset  0, 16 bytes */

    /* Pack-level measurements */
    float batteryVoltage;          /* offset 16,  4 bytes */
    float current;                 /* offset 20,  4 bytes  (charge current, A) */
    float dischargeCurrent;        /* offset 24,  4 bytes  (discharge current, A) */
    float dischargeVoltage;        /* offset 28,  4 bytes  (load voltage, V) */
    float temperature;             /* offset 32,  4 bytes  (Celsius) */

    /* Control signals / flags */
    uint8_t chargeSignal;          /* offset 36,  1 byte  (1 = no charger, 0 = charging) */
    uint8_t chargeEnable;          /* offset 37,  1 byte  (MOSFET state) */
    uint8_t dischargeEnable;       /* offset 38,  1 byte  (MOSFET state) */

    /* State of charge */
    float soc;                     /* offset 39,  4 bytes  (pack SOC, %) */
    float cellSOC[4];              /* offset 43, 16 bytes  (per-cell SOC, %) */
    float soh;                     /* offset 59,  4 bytes  (state of health, %) */

    /* Fault & state */
    uint16_t faults;               /* offset 63,  2 bytes  (bitmask, see BMS_FAULT_*) */
    uint8_t  state;                /* offset 65,  1 byte   (0 = idle, 1 = charging, 2 = discharging, 3 = fault, 4 = balancing) */
    uint8_t  balanceState[4];      /* offset 66,  4 bytes  (per-cell balance state, 0/1) */
    uint8_t  _reserved[10];         /* offset 70,  6 bytes  (padding to 80 bytes)     */

} BMS_Payload_t;

/* -------------------------------------------------------------------------- */
/*  Public API                                                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Build a binary BMS packet into tx_buf and send it over huart.
 *        Call this once per main-loop cycle (every 2 s).
 *
 * @param huart   UART handle to transmit on
 */
void BMS_Transmit(UART_HandleTypeDef *huart);

#endif /* INC_BMS_PROTOCOL_H_ */
