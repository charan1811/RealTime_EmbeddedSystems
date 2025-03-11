#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"
#include "inc/hw_ints.h"
#include "nrf.h"
#include "BMP/bmp180_sensor.h"
#include "MPU6050/mpu6050_sensor.h"
#include "I2C/i2c.h"
#include "UART/uart.h"

#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_i2c.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"

#define SEQ_PERIOD      (50000)
#define LOAD            (5)

SemaphoreHandle_t xSem1;
SemaphoreHandle_t xSem2;
SemaphoreHandle_t xSem3;

//#define regADD 0xAA

uint32_t clk;
long int p_arr[3];
int alert_send = 0;


struct sensor_struct sensor_data[4]= {0, 0, 0, 0};
TickType_t xStartTick =0;
TickType_t xCurrentTick = 0;
TickType_t delta = 0;


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

void ssi0_init(void)
{
    // Enable the SSI0 and GPIOA peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the SSI0 pins for use
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2);

    // Configure the SSI0 module for SPI master mode
    SSIConfigSetExpClk(SSI0_BASE, 50000000, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 10000, 8);

    // Set Pin 1 of Port B as GPIO output
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_2);

    // Enable the SSI0 module
    SSIEnable(SSI0_BASE);
}



void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    while(1)
    {
    }
}



uint32_t ReadTemp(uint32_t R_slave_address, uint8_t regadd)
{

    //specify that we are writing (a register address) to the
    //slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, R_slave_address, false);
    //SysCtlDelay(500);

    //specify register to be read
    I2CMasterDataPut(I2C0_BASE, regadd);
    //SysCtlDelay(500);

    //send control byte and register address byte to slave device
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    SysCtlDelay(50);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, R_slave_address, true);

    //send control byte and read from the register we
    //specified
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //return data pulled from the specified register
    return I2CMasterDataGet(I2C0_BASE);

}


void data_capt()
{
    while(1)
    {
        xSemaphoreTake(xSem1, portMAX_DELAY);
        xStartTick = xTaskGetTickCount();
        //Take pressure values
        BMP_callibration_sequence();
        //Take Gyro values
        GYRO_callibration_sequence();
        xCurrentTick = xTaskGetTickCount();
        delta = xCurrentTick - xStartTick;
    }
}

//detection task
void detection()
{
    uint32_t bankingAngle = 30;
    uint32_t frictionCoefficient = 2;
    while(1)
    {
        xSemaphoreTake(xSem2, portMAX_DELAY);
        uint32_t avg_pressure = 0;
        int i =0;

        // Sum up the pressure values in the buffer
        for (i = 0; i < 4; i++)
        {
            avg_pressure = avg_pressure + sensor_data[i].p;
        }

        // Calculate the average by dividing the sum by the number of samples
        avg_pressure = avg_pressure / 15;

        // Return the computed average pressure
        // Threshold for detecting a significant pressure drop
        int pressure_threshold = 70;

        // Assuming simple friction model: v_max = sqrt(g * R * tan(theta) * friction_coefficient)
        float gravity = 9.81; // m/s^2, acceleration due to gravity
        float radius = 10.0;  // assumed radius of the curve in meters

        // Calculate the maximum speed
        float maxSpeed = gravity * radius * bankingAngle * frictionCoefficient;

        // Convert meters per second to miles per hour
        float maxSpeed_mph = maxSpeed * 2.23694;

        // Limit the maximum speed to 100 mph (about 44.7 m/s)
        maxSpeed = fmin(maxSpeed_mph, 100);

        // Compare the current pressure with the average pressure
        if (MAX_SPEED > maxSpeed)
        {
            alert_send = 1; // Set the BMP_flag to 1, indicating a potential accident
        }
        else
        {
            alert_send = 0;
        }

        // Compare the current pressure with the average pressure
        if (PRESSURE_THRESH < (avg_pressure - pressure_threshold))
        {
            alert_send = 1; // Set the BMP_flag to 1, indicating a potential accident
	    xSemaphoreGive(xSem3);
        }
        else
        {
            alert_send = 0;
        }
    }
}

void alert_send_task()
{
    xSemaphoreTake(xSem3, portMAX_DELAY);
    if (alert_send)
    {
        nrf_send('1');
    }
}

void InterruptHandler(TimerHandle_t xTimer)
{
    static uint32_t seqCnt=0;
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Release each service at a sub-rate of the generic sequencer rate
   UARTprintf("S%u\n", seqCnt);

    if((seqCnt) == 0)
    {
        xSemaphoreGive(xSem1);
    }

    if((seqCnt % 25) == 0)
    {
        xSemaphoreGive(xSem2);
    }

    seqCnt++;

    if (seqCnt == 30)
    {
        seqCnt = 0;
    }
}

void main(void)
{
// Set the clocking to run directly from the external crystal/oscillator.
// Initialize the UART and configure it for 115200
    clk = SysCtlClockFreqSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ, 16000000);
    ConfigureUART();

    UARTprintf("data1: %d\n", clk);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Enable processor interrupts
    ROM_IntMasterEnable();
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    TimerLoadSet(TIMER0_BASE, TIMER_A, SEQ_PERIOD);

    TimerIntRegister(TIMER0_BASE, TIMER_A, InterruptHandler);

    IntEnable(INT_TIMER0A);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    TimerEnable(TIMER0_BASE, TIMER_A);
    // Enable the clock for Port B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

        // Set Pin 1 of Port B as GPIO output
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1);

        // Optionally, disable analog functionality on Pin 1
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_1);
    ssi0_init();

    //initialize I2C module 0 and 2
    InitI2C0();
    InitI2C2();

    //initialize BMP and MPU
    init_BMP180();
    MPU6050_Init();

    xSem1 = xSemaphoreCreateBinary();
    xSem2 = xSemaphoreCreateBinary();
    xSem3 = xSemaphoreCreateBinary();

    //task 1 creation - Data Aquisition
    if(xTaskCreate(data_capt, "Task1", (2*configMINIMAL_STACK_SIZE), NULL, configMAX_PRIORITIES -1 , NULL) != pdTRUE)
    {
        UARTprintf("Unable to create data_capt\n");
        while(1);
    }

    //task 2 creation - Accident detection
    if(xTaskCreate(detection, "Task2", (2*configMINIMAL_STACK_SIZE), NULL, configMAX_PRIORITIES -2 , NULL) != pdTRUE)
    {
        UARTprintf("Unable to create detection\n");
        while(1);
    }

    //task 2 creation - Accident detection
    if(xTaskCreate(alert_send_task, "Task3", (2*configMINIMAL_STACK_SIZE), NULL, configMAX_PRIORITIES -2 , NULL) != pdTRUE)
    {
        UARTprintf("Unable to create detection\n");
        while(1);
    }

    vTaskStartScheduler();

    while(1)
    {
    }
}
