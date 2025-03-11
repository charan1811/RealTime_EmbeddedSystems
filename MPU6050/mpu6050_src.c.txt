#include "mpu6050.h"
#include "i2c.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"

/*
 * Function: MPU6050_Init
 * Description: Initializes the MPU6050 sensor.
 */
void MPU6050_Init()
{
    // Reset device
    I2C2_Write(PWR_MGMT_1, DEVICE_RESET_BIT); // Set bit 7 to 1 for device reset
    //for (int i = 0; i < 5000; i++);                           // Wait for device reset to complete
    SysCtlDelay(5000);

    // Wake up device and set clock source
     I2C2_Write(PWR_MGMT_1, WAKE_UP_BIT);      // Clear bit 6 (SLEEP) to wake up
     SysCtlDelay(50);
    I2C2_Write(PWR_MGMT_1, CLKSEL_PLL_XGYRO); // Set bit 0 (CLKSEL) for PLL with X-axis gyro reference
    SysCtlDelay(50);
    // Set full-scale range to +/- 2g (adjust as needed)
     I2C2_Write(ACCEL_CONFIG, FULL_SCALE_RANGE_2G); // AFS_SEL = 0 ( 2g)
     SysCtlDelay(50);
    // Set accelerometer digital low pass filter to 44 Hz
     I2C2_Write(ACCEL_CONFIG_2, ACCEL_DLPF_44HZ);  // ACCEL_FCHOICE_B = 1, A_DLPF_CFG = 1
}

/*
 * Function: GYRO_callibration_sequence
 * Description: Reads accelerometer values and performs calibration.
 */
void GYRO_callibration_sequence()
{

    // Read accelerometer values along the X, Y, and Z axes
    int32_t ACx_H = readgyro(MPU6050_ADDRESS, ACCEL_XOUT_H);
    SysCtlDelay(1000);
    int32_t ACx_L = readgyro(MPU6050_ADDRESS, ACCEL_XOUT_L);
    uint16_t x = ((ACx_H << 8) | ACx_L);

    //for (int i = 0; i < CALIBRATION_SAMPLES; i++);
    SysCtlDelay(1000);

    int32_t ACy_H = readgyro(MPU6050_ADDRESS, ACCEL_YOUT_H);
    SysCtlDelay(1000);
    int32_t ACy_L = readgyro(MPU6050_ADDRESS, ACCEL_YOUT_L);
    uint16_t y = ((ACy_H << 8) | ACy_L);

    //for (int i = 0; i < CALIBRATION_SAMPLES; i++);
    SysCtlDelay(1000);

    int32_t ACz_H = readgyro(MPU6050_ADDRESS, ACCEL_ZOUT_H);
    SysCtlDelay(1000);
    int32_t ACz_L = readgyro(MPU6050_ADDRESS, ACCEL_ZOUT_L);
    uint16_t z = ((ACz_H << 8) | ACz_L);

    SysCtlDelay(1000);

    UARTprintf("\nx = %u\t y = %u\t z = %u\n", x,y,z);

}

//I2C2, Read gyro
uint32_t readgyro(uint32_t R_slave_address, uint8_t regadd)
{

    //specify that we are writing (a register address) to the
    //slave device
    I2CMasterSlaveAddrSet(I2C2_BASE, R_slave_address, false);
    SysCtlDelay(50);

    //specify register to be read
    I2CMasterDataPut(I2C2_BASE, regadd);
    SysCtlDelay(50);

    //send control byte and register address byte to slave device
    //I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    SysCtlDelay(50);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C2_BASE));

    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C2_BASE, R_slave_address, true);
    SysCtlDelay(50);
    //send control byte and read from the register we
    //specified
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    SysCtlDelay(50);
    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C2_BASE));
    SysCtlDelay(50);
    //return data pulled from the specified register
    return I2CMasterDataGet(I2C2_BASE);

}
