/*
 * RF Communication header file
 * Author: Akash Karoshi (modified for Tiva TM4C1239NCPDTXL)
 * Date: 12/08/2023
 */
#ifndef RFCOM_H
#define RFCOM_H

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"

// NRF24L01 Register Addresses
enum nrf_reg
{
    // Command registers
    R_REGISTER = 0x00,  // Read command
    nrf_read = 0x1F,    // Read command
    nrf_write = 0x20,   // Write command

    R_RX_PAYLOAD = 0x61,  // Read RX payload command

    FLUSH_TX = 0xE1,  // Flush TX FIFO command
    FLUSH_RX = 0xE2,  // Flush RX FIFO command

    R_RX_PL_WID = 0x60,  // Read RX payload width command
    W_TX_PAYLOAD = 0xA0,  // Write TX payload command

    // Address registers
    NRF_CONFIG = 0x00,  // Configuration register
    NRF_EN_AA = 0x01,  // Enable Auto Acknowledgment register
    NRF_RXADDR = 0x02,  // Enable RX addresses register
    NRF_RXADDR_P0_MASK = 0x3E,  // Enable RX address pipe 0 bit mask
    NRF_RXADDR_P0_SET = 0x01,  // Enable RX address pipe 0 bit set

    NRF_SETUP_AW = 0x03,  // Setup of Address Width register
    NRF_SETUP_RETR = 0x04,  // Setup of Automatic Retransmission register
    NRF_RF_CH = 0x05,  // RF Channel register
    NRF_SETUP = 0x06,  // Setup register
    NRF_STATUS = 0x07,  // Status register
    NRF_OBSERVE_TX = 0x08,  // Transmit observe register
    NRF_RPD = 0x09,  // Received Power Detector register

    NRF_ADDR_P0 = 0x0A,  // RX address pipe 0
    NRF_ADDR_P1 = 0x0B,  // RX address pipe 1
    NRF_ADDR_P2 = 0x0C,  // RX address pipe 2
    NRF_ADDR_P3 = 0x0D,  // RX address pipe 3
    NRF_ADDR_P4 = 0x0E,  // RX address pipe 4
    NRF_ADDR_P5 = 0x0F,  // RX address pipe 5

    NRF_TX_ADDR = 0x10,  // TX address
    NRF_RX_PW_P0 = 0x11,  // RX payload width pipe 0
    NRF_RX_PW_P0_MASK = 0x3F,  // RX payload width pipe 0 bit mask
    NRF_RX_PW_P0_SET = 0x01,  // RX payload width pipe 0 bit set
    NRF_RX_PW_SET_MASK = 0x00,  // RX payload width bits mask
    NRF_RX_PW_P1 = 0x12,  // RX payload width pipe 1
    NRF_RX_PW_P2 = 0x13,  // RX payload width pipe 2
    NRF_RX_PW_P3 = 0x14,  // RX payload width pipe 3
    NRF_RX_PW_P4 = 0x15,  // RX payload width pipe 4
    NRF_RX_PW_P5 = 0x16,  // RX payload width pipe 5

    NRF_FIFO_STATUS = 0x17,  // FIFO Status register
    NRF_DYNPD = 0x1C,  // Dynamic Payload enable register
    NRF_FEATURE = 0x1D,  // Feature register

    // Setup registers
    POWER_UP = 0x02,  // Power up command
    POWER_DOWN = 0x00,  // Power down command
    POWER_MASK = 0xFD,  // Power down bit mask

    PRIM_RX = 1,  // RX mode bit
    PRIM_TX = 0,  // TX mode bit
    PRIM_RX_MASK = 0xFE,  // PRIM_RX bit mask

    // New Registers
    EN_DPL = 0x04,  // Enable Dynamic Payload feature
    EN_DPL_MASK = 0xFB,  // EN_DPL bit mask

    EN_DPL_P0 = 1,  // Enable Dynamic Payload on pipe 0
    EN_DPL_P0_MASK = 0xFE,  // EN_DPL_P0 bit mask

    RX_DR_SET = 0x40,  // Data Ready RX FIFO interrupt bit set
    RX_DR_MASK = 0xBF,  // RX_DR bit mask

    MAX_RT_SET = 0x10,  // Maximum number of TX retries interrupt bit set
    MAX_RT_MASK = 0xEF,  // MAX_RT bit mask

    TX_DS_SET = 0x20,  // Data Sent TX FIFO interrupt bit set
    TX_DS_MASK = 0xDF,  // TX_DS bit mask

    ADR_4000 = 0xF0,  // Address field width 4 bytes
    ADR_MASK = 0x0F,  // Address field width bits mask

    RF_PWR_18db = 0x00,  // RF output power in TX mode 0 dBm
    RF_PWR_MASK = 0xF9,  // RF_PWR bits mask

    RF_DR_LOW = 0x20,  // Set RF data rate to 250 kbps
    RF_DR_MASK = 0xD7,  // RF_DR bits mask

    ARD_750us = 0x1A,  // Automatic Retransmission Delay 750 us
    ARD_MASK = 0xF0,  // ARD bits mask

    ARC_15_RETRY = 0x0F,  // Maximum number of TX retransmits 15
    ARC_MASK = 0x0F,  // ARC bits mask

    NRF_ENAA_P0 = 0x01,  // Enable Auto Acknowledgment pipe 0
    NRF_ENAA_P0_MASK = 0x3E,  // ENAA_P0 bit mask

    NRF_ERX_P0 = 0x01,  // Enable RX Addresses pipe 0
    NRF_ENRX_P0_MASK = 0x3E,  // ENRX_P0 bit mask

    NRF_AW_BYTES = 0x02,  // Set address width to 5 bytes
    NRF_AW_MASK = 0x03,  // AW bits mask

    NRF_RF_CH_NUMBER = 0x6E,  // Set RF channel to 76
    NRF_RF_CH_MASK = 0x7F,  // RF_CH bits mask

    RF_PWR_00db = 0x03,  // RF output power in TX mode -6 dBm

    RF_TX_ADDR = 0xE7,  // Set TX address to 0xE7E7E7E7E7
    RF_TX_ADDR_MASK = 0xFF,  // TX_ADDR bits mask

    RF_CONFIG_CRCO = 0x04,  // Enable 16-bit CRC
    RF_ENABLE_CRC = 0x08,  // Enable CRC
    NRF_CONFIG_MASK = 0x71,  // Configuration bits mask
    NRF_NOP = 0xFF,  // No Operation command
};

/*
 * RF Initialization
 */
void rf_INIT(void);

/*
 * RF Read
 * Parameters:
 *   register_add: Register address to read from
 * Returns:
 *   Value read from the specified register
 */
char nrf_cmd_rd(char register_add);

/*
 * RF Write
 * Parameters:
 *   register_add: Register address to write to
 *   new_value: New value to write to the register
 * Returns:
 *   Value read from the specified register after write operation
 */
char nrf_cmd_wr(char register_add, char new_value);

/*
 * Delay in milliseconds
 * Parameters:
 *   count: Number of milliseconds to delay
 */
void DelayMs(uint16_t count);

/*
 * Simple command for NRF24
 * Parameters:
 *   cmd: Command to send
 * Returns:
 *   Response received after sending the command
 */
char nrf_cmd(char cmd);

/*
 * NRF24 TX initialization
 */
void nrf_ptx_init(void);

/*
 * Send data using NRF24
 * Parameters:
 *   payload: Data payload to send
 * Returns:
 *   0 if successful, 1 if unsuccessful
 */
uint8_t nrf_send(char payload);

/*
 * NRF24 RX initialization
 * returns: none
 * Parameters: none
 */
void rf_rx_INIT(void);

/*
 * Receive data using NRF24
 * Parameters:
 *   payload: Buffer to store received data
 */
void nrf_rf_rx(char *payload);

#endif  // RFCOM_H
