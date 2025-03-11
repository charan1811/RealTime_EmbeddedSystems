#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
// Register addresses for accelerometer data
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

// Register addresses for configuration
#define PWR_MGMT_1 0x6B
#define ACCEL_CONFIG 0x1C
#define ACCEL_CONFIG_2 0x1D

// Other constants for MPU6050
#define DEVICE_RESET_BIT 0x80
#define WAKE_UP_BIT 0x00
#define CLKSEL_PLL_XGYRO 0x01
#define FULL_SCALE_RANGE_2G 0x00
#define ACCEL_DLPF_44HZ 0x01

// Define the I2C address of the MPU6050
#define MPU6050_ADDRESS 0x68  //0xD0

//can change this to hold average values
struct sensor_struct
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
    long int p;
};

void MPU6050_Init();

void GYRO_callibration_sequence();

uint32_t readgyro(uint32_t R_slave_address, uint8_t regadd);

#endif
