#ifndef MCF8316A_REGISTERS_H
#define MCF8316A_REGISTERS_H

#include <Arduino.h>

// I2C Control Word Definitions
#define MCF8316A_RW_READ            (1 << 23)
#define MCF8316A_RW_WRITE           (0 << 23)
#define MCF8316A_CRC_ENABLE         (1 << 22)
#define MCF8316A_CRC_DISABLE        (0 << 22)
#define MCF8316A_DATA_LEN_16BIT     (0 << 20)
#define MCF8316A_DATA_LEN_32BIT     (1 << 20)
#define MCF8316A_DATA_LEN_64BIT     (2 << 20)

// Memory Map Sections
#define MCF8316A_SECTION_MASK       0x000F0000
#define MCF8316A_PAGE_MASK          0x0000FF00
#define MCF8316A_ADDRESS_MASK       0x000000FF

// EEPROM Registers (Non-volatile) - Base address 0x80
#define MCF8316A_ISD_CONFIG         0x00000080
#define MCF8316A_REV_DRIVE_CONFIG   0x00000082
#define MCF8316A_MOTOR_STARTUP1     0x00000084
#define MCF8316A_MOTOR_STARTUP2     0x00000086
#define MCF8316A_CLOSED_LOOP1       0x00000088
#define MCF8316A_CLOSED_LOOP2       0x0000008A
#define MCF8316A_CLOSED_LOOP3       0x0000008C
#define MCF8316A_CLOSED_LOOP4       0x0000008E
#define MCF8316A_FAULT_CONFIG1      0x00000090
#define MCF8316A_FAULT_CONFIG2      0x00000092
#define MCF8316A_SPEED_PROFILES1    0x00000094
#define MCF8316A_SPEED_PROFILES2    0x00000096
#define MCF8316A_SPEED_PROFILES3    0x00000098
#define MCF8316A_SPEED_PROFILES4    0x0000009A
#define MCF8316A_SPEED_PROFILES5    0x0000009C
#define MCF8316A_SPEED_PROFILES6    0x0000009E
#define MCF8316A_INT_ALGO_1         0x000000A0
#define MCF8316A_INT_ALGO_2         0x000000A2
#define MCF8316A_PIN_CONFIG         0x000000A4
#define MCF8316A_DEVICE_CONFIG1     0x000000A6
#define MCF8316A_DEVICE_CONFIG2     0x000000A8
#define MCF8316A_PERI_CONFIG1       0x000000AA
#define MCF8316A_GD_CONFIG1         0x000000AC
#define MCF8316A_GD_CONFIG2         0x000000AE
#define MCF8316A_CONFIG_DATA_1TO8   0x000000B0
#define MCF8316A_CONFIG_DATA_9TO16  0x000000B2
#define MCF8316A_I2C_TARGET_ADDR    0x000000E0

// RAM Registers (Volatile) - Base address 0xE0
#define MCF8316A_GATE_DRIVER_FAULT_STATUS   0x000000E0
#define MCF8316A_CONTROLLER_FAULT_STATUS    0x000000E2
#define MCF8316A_ALGO_STATUS               0x000000E4
#define MCF8316A_MTR_PARAMS                0x000000E6
#define MCF8316A_ALGO_STATUS_MPET          0x000000E8
#define MCF8316A_STAT_STATUS               0x000000EA
#define MCF8316A_DEV_CTRL                  0x000000EA
#define MCF8316A_MOTOR_SPEED               0x000000EC
#define MCF8316A_DC_BUS_CURRENT            0x000000F0
#define MCF8316A_SPEED_CONTROL             0x000000F2
#define MCF8316A_ALGO_CTRL                 0x000000F4
#define MCF8316A_DAC_STATUS                0x000001E0
#define MCF8316A_DEVICE_ID                 0x000001E2
#define MCF8316A_EEPROM_WR_DONE            0x000001E4

// Algorithm States
#define MCF8316A_ALGO_STATE_IDLE            0x00
#define MCF8316A_ALGO_STATE_STARTUP         0x01
#define MCF8316A_ALGO_STATE_CLOSED_LOOP     0x02
#define MCF8316A_ALGO_STATE_FAULT           0x03

// Control Commands (DEV_CTRL register)
#define MCF8316A_CTRL_RUN_MOTOR             (1 << 0)
#define MCF8316A_CTRL_DIRECTION             (1 << 1)
#define MCF8316A_CTRL_BRAKE                 (1 << 2)
#define MCF8316A_CTRL_CLR_FAULT             (1 << 3)
#define MCF8316A_CTRL_SPEED_MODE            (1 << 4)
#define MCF8316A_CTRL_TORQUE_MODE           (0 << 4)
#define MCF8316A_CTRL_SAVE_TO_EEPROM        (1 << 8)
#define MCF8316A_CTRL_LOAD_FROM_EEPROM      (1 << 9)

// Fault Status Bits
#define MCF8316A_FAULT_UVLO                 (1 << 0)
#define MCF8316A_FAULT_OCP                  (1 << 1)
#define MCF8316A_FAULT_OT                   (1 << 2)
#define MCF8316A_FAULT_LOCK                 (1 << 3)
#define MCF8316A_FAULT_IPD                  (1 << 4)
#define MCF8316A_FAULT_ABN_SPEED            (1 << 5)
#define MCF8316A_FAULT_NO_MOTOR             (1 << 6)

// Speed Control Settings
#define MCF8316A_SPEED_RANGE_120HZ          0x00
#define MCF8316A_SPEED_RANGE_400HZ          0x01
#define MCF8316A_SPEED_RANGE_800HZ          0x02
#define MCF8316A_SPEED_RANGE_1200HZ         0x03

// Motor Direction
#define MCF8316A_DIR_FORWARD                0
#define MCF8316A_DIR_REVERSE                1

// CRC-8 Polynomial for optional error checking
#define MCF8316A_CRC8_POLY                  0x07  // x^8 + x^2 + x + 1
#define MCF8316A_CRC8_INIT                  0xFF

#endif // MCF8316A_REGISTERS_H