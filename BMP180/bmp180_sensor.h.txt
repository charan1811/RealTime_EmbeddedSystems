#ifndef BMP180_H
#define BMP180_H

#include <stdint.h>

// Macros for BMP180 Calibration Register Addresses
#define BMP180_REG_AC1 0xAA // Address of AC1 calibration register
#define BMP180_REG_AC2 0xAC // Address of AC2 calibration register
#define BMP180_REG_AC3 0xAE // Address of AC3 calibration register
#define BMP180_REG_AC4 0xB0 // Address of AC4 calibration register
#define BMP180_REG_AC5 0xB2 // Address of AC5 calibration register
#define BMP180_REG_AC6 0xB4 // Address of AC6 calibration register
#define BMP180_REG_B1  0xB6 // Address of B1 calibration register
#define BMP180_REG_B2  0xB8 // Address of B2 calibration register
#define BMP180_REG_MB  0xBA // Address of MB calibration register
#define BMP180_REG_MC  0xBC // Address of MC calibration register
#define BMP180_REG_MD  0xBE // Address of MD calibration register

// Macros for BMP180 Control and Measurement Registers
#define BMP180_REG_CTRL_MEAS  0xF4 // Address of Control Measurement Register
#define BMP180_REG_OUT_MSB    0xF6 // Address of Output MSB Register

#define PRESSURE_THRESH 394758

#define DEVICE_ADDRESS 0x77
#define W_DEVICE_ADDRESS 0x77
#define MAX_SPEED 425346

struct calib_struct
{
    short int ACA1;
    short int ACA2;
    short int ACA3;
    unsigned short int ACA4;
    unsigned short int ACA5;
    unsigned short int ACA6;
    short int B1;
    short int B2;
    short int MB;
    short int MC;
    short int MD;
};

struct calib_struct bmp_calib;

void init_BMP180();

void BMP_callibration_sequence();

#endif
