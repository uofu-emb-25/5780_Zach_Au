#include <stdint.h>
#include <stdlib.h>
#include "main.h"
#include "hal_gpio.h"
#include <stm32f0xx_hal.h>

// System clock configuration function prototype
void SystemClock_Config(void);

// Inline function to update LED state: only one LED on at a time.
static inline void setLED(uint32_t ledPos) {
    GPIOC->ODR = (GPIOC->ODR & ~((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9))) | (1 << ledPos);
}

int checkI2CStatusFlag(int flagBit) {
    // Wait until either NACKF (bit 4) or the specified flag is set
    while (!(I2C2->ISR & (1 << 4)) && !(I2C2->ISR & (1 << flagBit))) {}
    if (I2C2->ISR & (1 << 4)) {
        // If NACKF is set, indicate error by turning on the red LED (PC6)
        GPIOC->ODR |= (1 << 6);
        return 0;
    }
    return 1;
}

void gyroscope(int addr, int count, int *buffer, int isRead, int regAddr) {
    const uint32_t NUM_BYTES_POS = 16;   // Bit position for number of bytes in CR2
    const uint32_t SLAVE_ADDR_POS = 1;     // Bit position for slave address in CR2
    const uint32_t AUTO_INCREMENT = 128;   // Auto-increment flag for register address
    
    // Clear previous transaction settings (NBYTES, slave address, and RD_WRN flag)
    I2C2->CR2 &= ~((0xFF << NUM_BYTES_POS) | (0x3FF) | (1 << 10));
    
    if (isRead) {
        // For read: initially set to send only the register address
        I2C2->CR2 |= (1 << NUM_BYTES_POS) | (addr << SLAVE_ADDR_POS);
    }
    else {
        // For write: count includes register address plus data bytes
        I2C2->CR2 |= ((count + 1) << NUM_BYTES_POS) | (addr << SLAVE_ADDR_POS);
    }
    
    // Start the I2C transaction by setting the START bit
    I2C2->CR2 |= (1 << 13);
    
    if (checkI2CStatusFlag(1)) {  // Check TXIS flag
        // Send the register address with auto-increment if needed
        I2C2->TXDR = regAddr | ((count > 1) ? AUTO_INCREMENT : 0);
        
        if (isRead) {
            // Wait until register address transmission completes
            while (!(I2C2->ISR & (1 << 6))) {};
            
            // Reconfigure CR2 for the reading phase:
            I2C2->CR2 &= ~((0xFF << NUM_BYTES_POS) | (0x3FF));
            I2C2->CR2 |= (count << NUM_BYTES_POS) | (addr << SLAVE_ADDR_POS) | (1 << 10);
            
            // Restart transaction for read
            I2C2->CR2 |= (1 << 13);
            
            // Wait for RXNE flag
            for (int i = 0; i < count; i++) {
                while (!checkI2CStatusFlag(2));
                buffer[i] = I2C2->RXDR;
            }
        }
        else {
            // Wait for TXIS flag
            for (int i = 0; i < count; i++) {
                while (!checkI2CStatusFlag(1));
                I2C2->TXDR = buffer[i];
            }
        }
    }
    
    // Wait until the transaction is complete
    while (!(I2C2->ISR & (1 << 6)));
    
    // End the I2C transaction by setting the STOP bit
    I2C2->CR2 |= (1 << 14);
}

int lab5_main(void)
{
    HAL_Init();
    SystemClock_Config();

    // Enable GPIOB and GPIOC clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
    
    // Enable I2C2 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    
    // Configure LEDs
    GPIOC->MODER |= (1 << 12); // PC6: Red LED
    GPIOC->MODER |= (1 << 14); // PC7: Blue LED
    GPIOC->MODER |= (1 << 16); // PC8: Orange LED
    GPIOC->MODER |= (1 << 18); // PC9: Green LED

    // Configure PB11
    GPIOB->MODER |= (1 << 23);
    GPIOB->MODER &= ~(1 << 22);
    GPIOB->OTYPER |= (1 << 11);
    GPIOB->AFR[1] |= (1 << 12);
    
    // Configure PB13
    GPIOB->MODER |= (1 << 27);
    GPIOB->MODER &= ~(1 << 26);
    GPIOB->OTYPER |= (1 << 13);
    GPIOB->AFR[1] |= (1 << 20);
    GPIOB->AFR[1] &= ~(1 << 21);
    GPIOB->AFR[1] |= (1 << 22);
    GPIOB->AFR[1] &= ~(1 << 23);
    
    // Configure PB14
    GPIOB->MODER &= ~(1 << 29);
    GPIOB->MODER |= (1 << 28);
    GPIOB->OTYPER &= ~(1 << 14);
    GPIOB->ODR |= (1 << 14); // Set to high
    
    // Configure PC0 for SPI/I2C mode selection (push-pull)
    GPIOC->MODER |= (1 << 0);
    GPIOC->ODR |= (1 << 0); // Set to high
    
    // Set I2C2 timing parameters 
    I2C2->TIMINGR = 0;  // clear
    I2C2->TIMINGR |= 0x13;
    I2C2->TIMINGR |= (0xF << 8);
    I2C2->TIMINGR |= (0x2 << 16);
    I2C2->TIMINGR |= (0x4 << 20);
    I2C2->TIMINGR |= (1 << 28);
    
    // Enable I2C2 peripheral
    I2C2->CR1 |= (1 << 0);
    
    // Gyroscope initialization
    int ctrlValue = 11;
    gyroscope(0x69, 1, &ctrlValue, 0, 0x20);
    
    int sensorData[4];
    
    while (1)
    {
        HAL_Delay(100);
        gyroscope(0x69, 4, sensorData, 1, 0x28);
     
        int16_t xVal = (sensorData[1] << 8) | sensorData[0];
        int16_t yVal = (sensorData[3] << 8) | sensorData[2];
        
        if (yVal >= 500) {
            if (xVal >= 500)
                setLED((xVal > yVal) ? 9 : 6);
            else if (xVal <= -500)
                setLED((abs(xVal) > yVal) ? 8 : 6);
        } else if (yVal <= -500) {
            if (xVal <= -500)
                setLED((abs(xVal) > abs(yVal)) ? 8 : 7);
            else if (xVal >= 500)
                setLED((xVal > abs(yVal)) ? 9 : 7);
        }
    }
}