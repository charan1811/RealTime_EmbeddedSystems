#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void InitI2C0(void);
void InitI2C2(void);
void I2C_Write(uint8_t regadd, uint8_t data);

#endif
