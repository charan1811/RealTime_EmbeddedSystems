#include "bmp180.h"
#include "i2c.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"

/*
 * BMP Initialization Sequence
 * This function performs the initialization sequence for BMP180 sensor.
 * Parameters: None
 * Returns: None
 */
void init_BMP180()
{
    // Calibration coefficients read from BMP180
    bmp_calib.ACA1 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_AC1);

    bmp_calib.ACA2 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_AC2);

    bmp_calib.ACA3= ReadTemp(DEVICE_ADDRESS, BMP180_REG_AC3);

    bmp_calib.ACA4 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_AC4);

    bmp_calib.ACA5 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_AC5);

    bmp_calib.ACA6 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_AC6);

    bmp_calib.B1 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_B1);

    bmp_calib.B2 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_B2);

    bmp_calib.MB = ReadTemp(DEVICE_ADDRESS, BMP180_REG_MB);

    bmp_calib.MC = ReadTemp(DEVICE_ADDRESS, BMP180_REG_MC);

    bmp_calib.MD = ReadTemp(DEVICE_ADDRESS, BMP180_REG_MD);

}

/*
 * BMP Calibration Sequence
 * This function performs the calibration sequence for BMP180 sensor.
 * Parameters: None
 * Returns: None
 */
void BMP_callibration_sequence()
{
    int i = 0;
    // Temperature reading
    I2C_Write(BMP180_REG_CTRL_MEAS, 0x2E);
    for (i = 0; i < 5000; i++); // Delay for temperature conversion
    uint8_t A1 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_OUT_MSB); //ReadTemp(DEVICE_ADDRESS, BMP180_REG_OUT_MSB);
    //SysCtlDelay(500);
    uint8_t A2 = ReadTemp(DEVICE_ADDRESS, BMP180_REG_OUT_MSB+1);
    uint16_t temp = (A1 << 8) + A2;

    long int x1 = (temp - bmp_calib.ACA6) * bmp_calib.ACA5 / (2 << 15);
    long int x2 = bmp_calib.MC * (2 << 11) / (x1 + bmp_calib.MD);
    long int B5 = x1 + x2;

    // Pressure reading
    I2C_Write(BMP180_REG_CTRL_MEAS, (0x34 + (0 << 6)));
    for (i = 0; i < 5000; i++); // Delay for pressure conversion
    uint8_t PMSB = ReadTemp(DEVICE_ADDRESS, BMP180_REG_OUT_MSB);
    uint8_t PLSB = ReadTemp(DEVICE_ADDRESS, BMP180_REG_OUT_MSB+1);
    uint8_t PXLSB = ReadTemp(DEVICE_ADDRESS, BMP180_REG_OUT_MSB+2);
    long int UP = ((PMSB << 16) + (PLSB << 8) + PXLSB) >> (8 - 0);

    long int B6 = B5 - 4000;
    long int X1 = (bmp_calib.B2 * (B6 / (2 << 12))) / (2 << 11);
    long int X2 = (bmp_calib.ACA2 * B6) / (2 << 11);
    long int X3 = X1 + X2;
    long int B3 = (((bmp_calib.ACA1 * 4 + X3) << 0) + 2) / 4;
    unsigned long int B4 = bmp_calib.ACA4 * (unsigned long int)(X3 + 32768) / (2 << 15);
    unsigned long int B7 = ((unsigned long int)UP - B3) * (50000 >> 0);
    long int p;
    // Pressure compensation based on the value of B7
    if (B7 < 0x80000000)
    {
        p = (B7 * 2) / B4; // Positive pressure compensation
    }
    else
    {
        p = (B7 / B4) * 2; // Negative pressure compensation
    }

    // Additional calculations for pressure refinement
    X1 = (p / (2 << 8)) * (p / (2 << 8));
    X1 = (X1 * 3038) / (2 << 16);
    X2 = (-7357 * p) / (2 << 16);

    // Final pressure calculation incorporating refined values
    p = p + (X1 + X2 + 3791) / (2 << 4);
    UARTprintf("pressure: %u\n", p);

}
