// Write TX address to NRF24L01
void tx_addr_wr(char register_add, char new_value);
// Verify NRF24L01 configuration settings
void nrf_verify(void);

void csn_high(void)
{
    int i =0;
    for(i=0; i<100;i++);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

void csn_low(void)
{
    int i =0;
    for(i=0; i<100;i++);

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
}

void ce_high(void)
{
    int i =0;
    for(i=0; i<100;i++);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void ce_low(void)
{
    int i =0;
    for(i=0; i<100;i++);

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);
}

/* Initialize NRF24L01 for transmission
 * Returns: None
 * Parameters: None
 */
void rf_tx_INIT(void)
{
    ce_low();
    //SSIDataPut(SSI0_BASE, 0); // Dummy write to set CSN high
    csn_high();

    // Reset NRF24L01 configuration
    nrf_cmd_wr(NRF_CONFIG, 0);
    uint8_t res = nrf_cmd_rd(NRF_CONFIG);


    // Enable Auto Acknowledgment for Pipe 0
    nrf_cmd_wr(NRF_EN_AA, NRF_ENAA_P0);


    // Enable RX address for Pipe 0
    nrf_cmd_wr(NRF_RXADDR, NRF_ERX_P0);


    // Set address width to 5 bytes
    nrf_cmd_wr(NRF_SETUP_AW, 3);


    // Set Auto re-transmit delay to 750 microseconds
    nrf_cmd_wr(NRF_SETUP_RETR, 5);


    // Set RF Channel to 110
    //nrf_cmd_wr(NRF_RF_CH, RF_CHANNEL_NUM);
    nrf_cmd_wr(NRF_RF_CH, 110);


    // Set receiver payload width to 1 byte
    nrf_cmd_wr(NRF_RX_PW_P0, 1);


    // Set TX and RX addresses
    tx_addr_wr(NRF_TX_ADDR, RF_TX_ADDR);

    tx_addr_wr(NRF_ADDR_P0, RF_TX_ADDR);

    // Set RF setup to -18db, 1 Mega bytes
    nrf_cmd_wr(NRF_SETUP, 6);


    // Power up NRF24L01 and enable CRC
    nrf_cmd_wr(NRF_CONFIG, RF_ENABLE_CRC | RF_CONFIG_CRCO);

    char x = nrf_cmd_rd(NRF_CONFIG);

    nrf_cmd_wr(NRF_CONFIG, x | (1 << 1));


    //SysCtlDelay(10);
    DelayMs(100);
    SysCtlDelay(10000);

    nrf_verify();
    ce_high();
}

/* Verify NRF24L01 configuration settings
 * Returns: None
 * Parameters: None
 */
void nrf_verify()
{
    // Power UP
    char res = nrf_cmd_rd(NRF_CONFIG);
    // Enable AA
    char res1 = nrf_cmd_rd(NRF_EN_AA);
    // Enable RX address
    char res2 = nrf_cmd_rd(NRF_RXADDR);
    // Address width of 4 bytes
    char res3 = nrf_cmd_rd(NRF_SETUP_AW);
    // Set RX PW P0
    char res4 = nrf_cmd_rd(NRF_RX_PW_P0);
    // RF Channel select
    char res5 = nrf_cmd_rd(NRF_RF_CH);
    // TX address set
    char res6 = nrf_cmd_rd(NRF_TX_ADDR);
    // Rx address set
    char res7 = nrf_cmd_rd(NRF_ADDR_P0);
    // RF setup to 00db
    char res8 = nrf_cmd_rd(NRF_SETUP);
    // Retransmission register configured
    char res9 = nrf_cmd_rd(NRF_SETUP_RETR);

    char res10 = nrf_cmd_rd(0x04);

    char res11 = nrf_cmd_rd(NRF_FIFO_STATUS);
    //test the registers values
    if (res == NRF_CONFIG)
    {
        UARTprintf("CONFIG REGISTER : 0x15 \n\r");
    }
    if (res1 == NRF_ENAA_P0)
    {
        UARTprintf("AUTO ACKNOWLEDGEMENT ENABLE FOR PIPE IS: 1\n\r");
    }
    if (res3 == NRF_AW_BYTES)
    {
        UARTprintf("ADDRESS WIDTH IS CONFIGURED AS : 5 Bytes \n\r");
    }
    if (res4 == NRF_RX_PW_P0_SET)
    {
        UARTprintf("RX DEVICE PAYLOAD SIZE IS: 1 Byte\n\r");
    }
    if (res5 == 5)
    {
        UARTprintf("RX CHANNEL CONFIGURED AS : 110 \n\r");
    }
    if (res6 == RF_TX_ADDR)
    {
        UARTprintf("TX ADDRESS IS CONFIGURED AS 0xE7E7E7E7E7 \n\r");
    }
    if (res7 == NRF_RX_PW_P0)
    {
        UARTprintf("RX ADDRESS IS CONFIGURED AS 0xE7E7E7E7E7 \n\r");
    }
    if (res9 == 5)
    {
        UARTprintf("RETRANSMISSION REGISTER CONFIGURED WITH 0x2F\n\r");
    }
    if (res2)
    {
        UARTprintf("DYNAMIC PAYLOAD STATUS: DISABLED\n\r");
        UARTprintf("DATA TRANSMISSION RATE IS SET TO 1Mbps n\r");
        UARTprintf("TX OUPUT TRANSMISSION POWER IS SET TO 0x0 dB\n\r");
    }
}

/*
 * NRF24 RX initialization
 * returns: none
 * Parameters: none
 */
void rf_rx_INIT(void)
{
    ce_low();
    csn_high();

    SysCtlDelay(10);
    // Enable Auto Acknowledgment for Pipe 0
    nrf_cmd_wr(NRF_EN_AA, NRF_ENAA_P0);

    // Enable RX address for Pipe 0
    nrf_cmd_wr(NRF_RXADDR, NRF_ERX_P0);

    // Set address width to 4 bytes
    nrf_cmd_wr(NRF_SETUP_AW, NRF_AW_BYTES);

    // Set receiver payload width for Pipe 0
    nrf_cmd_wr(NRF_RX_PW_P0, NRF_RX_PW_P0_SET);

    // Set RF Channel to 80
    nrf_cmd_wr(NRF_RF_CH, 80);

    // Configure STATUS register
    nrf_cmd_wr(NRF_STATUS, (1 << 6) | (1 << 5) | (1 << 4));

    uint32_t data;
    csn_low();
    SSIDataPut(SSI0_BASE, FLUSH_RX);
    SSIDataGet(SSI0_BASE, &data);
    while(SSIBusy(SSI0_BASE));
    csn_high();

    csn_low();
    SSIDataPut(SSI0_BASE, FLUSH_TX);
    SSIDataGet(SSI0_BASE, &data);
    while(SSIBusy(SSI0_BASE));
    csn_high();

    char x = nrf_cmd_rd(NRF_CONFIG);

    nrf_cmd_wr(NRF_CONFIG, x | (1 << 1));

    // Set TX and RX addresses
    tx_addr_wr(NRF_TX_ADDR, RF_TX_ADDR);
    tx_addr_wr(NRF_ADDR_P0, RF_TX_ADDR);

    nrf_cmd_wr(NRF_CONFIG, x | (1 << 0));

    ce_high();

    nrf_verify();
}

/*
 * Function: tx_addr_wr
 * Description: Write TX address to NRF24L01.
 * Parameters:
 * - register_add: Register address to write to.
 * - new_value: Value to be written to the register.
 * Returns: None.
 */
void tx_addr_wr(char register_add, char new_value)
{
    int i;
    uint32_t x;
    // Create a command by combining write operation and register address
    char cmd;
    cmd = nrf_write | register_add;

    // Send the command and new value to the NRF24L01 for 5 times (5-byte address)
    csn_low();
    spi_transfer(cmd);

    for (i = 0; i < 5; i++)
    {
        spi_transfer(new_value);
    }
   csn_high();
}


char nrf_cmd_wr(char register_add, char new_value)
{
    csn_high();
    volatile char cmd;
    uint32_t x;
    cmd = nrf_write | register_add;
    csn_low();
    spi_transfer(cmd);
    spi_transfer(new_value);
    csn_high();
}

char spi_transfer(char value)
{
    uint32_t buffer;
    SSIDataPut(SSI0_BASE, (uint32_t)value);
    while(SSIBusy(SSI0_BASE));
    SSIDataGet(SSI0_BASE, &buffer);
    return ((char)(buffer & 0xFF));
}

char spi_dummy_transfer()
{
    return spi_transfer(0xFF);
}

char nrf_cmd_rd(char register_add)
{
    csn_high();
    char reg;
   csn_low();
   spi_transfer(register_add);
   reg = spi_dummy_transfer();
   csn_high();
    return reg;
}

/*
 * Function: DelayMs
 * Description: Implement a delay in milliseconds.
 * Parameters:
 * - count: Number of milliseconds to delay.
 */
void DelayMs(uint16_t count)
{
    int i=0, z =0;
    // Loop to create a delay
    for(i=0; i< count * 100; i++)
    {
        z++;
    }

}

/*
 * Function: nrf_ptx_init
 * Description: Initialize NRF24L01 for transmission.
 */
void nrf_ptx_init(void)
{
    // Initialize NRF24L01 for transmission by calling rf_tx_INIT function
    rf_tx_INIT();

    // Flush TX FIFO to ensure it is empty
    nrf_cmd(FLUSH_TX);
    //nrf_cmd(FLUSH_RX);

    // Send NOP command to ensure NRF24L01 is ready for further commands
    nrf_cmd(NRF_NOP);
}

/*
 * Function: nrf_cmd
 * Description: Send a simple command to NRF24L01.
 * Parameters:
 * - cmd: The command to be sent.
 * Returns: The response from the NRF24L01.
 */
char nrf_cmd(char cmd)
{
    char x;
    csn_low();
    spi_transfer(cmd);
    x = spi_dummy_transfer();
    csn_high();

    return x;
}

/*
 * Function: nrf_rf_tx
 * Description: Transmit data using NRF24L01.
 * Parameters:
 * - payload1: The data payload to be transmitted.
 */
void nrf_rf_tx(char payload1)
{
    // Temporary character and loop variable
    char c = 'a';
    int i = 0;

    // Flush TX FIFO
    nrf_cmd(FLUSH_TX);

    nrf_cmd(NRF_NOP);
    //csn_high();

    // Read NRF_CONFIG register to check the status
    char status = nrf_cmd_rd(NRF_CONFIG);
    //DelayMs(100);

    // Check if NRF24L01 is ready to transmit (TX FIFO not full and not transmitting)
    if (((status & 1) == 1) | ((status & 0x2) == 0))
    {
        // Initialize NRF24L01 in PTX mode
        nrf_ptx_init();
    }

    // Write TX payload command
    csn_low();
    spi_transfer(W_TX_PAYLOAD);
    spi_transfer(payload1);

    // Write the payload data to the TX FIFO
    csn_high();

    // Raise CE line to start data transmission
    ce_high();

    // Wait for a period (delay) to allow data transmission
    SysCtlDelay(100000);

    // Lower CE line to end data transmission
    ce_low();
}


/*
 * Function: nrf_send
 * Description: Send data using NRF24L01.
 * Parameters:
 * - payload: The data payload to be sent.
 * Returns: The status of the operation.
 */
uint8_t nrf_send(char payload)
{
    // Set initial status to 1 (success)
    uint8_t k = 1;

    // Transmit data using NRF24L01
    nrf_rf_tx(payload);

    // Read the status register
    // Read the status register from NRF24L01
        char status = nrf_cmd_rd(NRF_STATUS);

        // Check if data transmission is successful (TX_DS bit is set)
        if (status & 0x20)
        {
            // Set TX_DS bit and flush TX FIFO
            nrf_cmd_wr(NRF_STATUS, TX_DS_SET);
            nrf_cmd(FLUSH_TX);
            nrf_cmd(NRF_NOP);
            return 1; // Return success status
        }
        // Check if maximum retries reached (MAX_RT bit is set)
        else if (status & 0x10)
        {
            // Set MAX_RT bit and flush TX FIFO
            nrf_cmd_wr(NRF_STATUS, MAX_RT_SET);
           nrf_cmd(FLUSH_TX);
            return 0; // Return failure status
        }
        // Check if TX FIFO is full (TX_FULL bit is set)
        else if (status & 0x01)
        {
            // Flush TX FIFO
            nrf_cmd(FLUSH_TX);
            nrf_cmd(NRF_NOP);
            return 0; // Return failure status
        }
        else
        {
            return 0; // Return failure status (should not reach here)
        }
}
