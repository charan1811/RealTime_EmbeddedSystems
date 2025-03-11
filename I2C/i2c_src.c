// I2C Write Function
// Writes a byte of data to a specific register of an I2C device.
void I2C_Write(uint8_t regadd, uint8_t data)
{
    // Set slave address
    I2CMasterSlaveAddrSet(I2C0_BASE, DEVICE_ADDRESS, false);
    // Put reg address in transmit register
    I2CMasterDataPut(I2C0_BASE, regadd);
    // Send data
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    //SysCtlDelay(50);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));
    // Set slave address
    I2CMasterSlaveAddrSet(I2C0_BASE, DEVICE_ADDRESS, false);
    // Put data in transmit register
    I2CMasterDataPut(I2C0_BASE, data);
    // Send data
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    //I2CMasterControl(I2C0_BASE, I2C_MASTER_BURST_SEND_START);
    // Wait until transmission completes
    while (I2CMasterBusy(I2C0_BASE));
}

// I2C2 Initialization Function
// Configures and initializes I2C2 for communication.
void InitI2C2()
{

    //enable I2C module 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);

    //enable GPIO peripheral that contains I2C 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);                 //PL0 -sda and PL1 - scl

    // Configure the pin muxing for I2C2 functions on port L0 and L1
    GPIOPinConfigure(GPIO_PL1_I2C2SCL);
    GPIOPinConfigure(GPIO_PL0_I2C2SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTL_BASE, GPIO_PIN_1);
    GPIOPinTypeI2C(GPIO_PORTL_BASE, GPIO_PIN_0);

    // Enable and initialize the I2C2 master module. Use the system clock for
    // the I2C2 module. The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C2_BASE, clk, false);

    //clear I2C FIFOs
    I2CTxFIFOFlush(I2C2_BASE);

}

// Writes a byte of data to a register of the MPU6050 sensor via I2C2.
//I2C2 Write - MPU6050_ADDRESS
void I2C2_Write(uint8_t regadd, uint8_t data)
{
    // Set slave address
    I2CMasterSlaveAddrSet(I2C2_BASE, MPU6050_ADDRESS, false);
    SysCtlDelay(50);
    // Put reg address in transmit register
    I2CMasterDataPut(I2C2_BASE, regadd);
    SysCtlDelay(50);
    // Send data
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    SysCtlDelay(50);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C2_BASE));
    SysCtlDelay(50);
    // Set slave address
    I2CMasterSlaveAddrSet(I2C2_BASE, MPU6050_ADDRESS, false);
    SysCtlDelay(50);
    // Put data in transmit register
    I2CMasterDataPut(I2C2_BASE, data);
    SysCtlDelay(50);
    // Send data
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    SysCtlDelay(50);

    while (I2CMasterBusy(I2C2_BASE));

    SysCtlDelay(100);
}

// I2C0 Initialization Function
// Configures and initializes I2C0 for communication.
void InitI2C0()
{

    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    //enable GPIO peripheral that contains I2C 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable and initialize the I2C0 master module. Use the system clock for
    // the I2C0 module. The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, clk, false);

    //clear I2C FIFOs
    I2CTxFIFOFlush(I2C0_BASE);

}