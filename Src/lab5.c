#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <stdint.h>
#include <stdlib.h>
#include "main.h" // main header file

void SystemClock_Config(void);

uint8_t gyroSlaveAddr = 0x69; // 7-bit slave address (when PB14 is HIGH)
uint8_t whoAmIReg = 0x0F; // WHO_AM_I register address
uint8_t expectedWhoAmI = 0xD4; // Expected WHO_AM_I value

// Configures and initiates an I2C transaction with the gyroscope
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
            while (!(I2C2->ISR & (1 << 6)));
            
            // Reconfigure CR2 for the reading phase:
            I2C2->CR2 &= ~((0xFF << NUM_BYTES_POS) | (0x3FF));
            I2C2->CR2 |= (count << NUM_BYTES_POS) | (addr << SLAVE_ADDR_POS) | (1 << 10);
            
            // Restart transaction for read
            I2C2->CR2 |= (1 << 13);
            
            for (int i = 0; i < count; i++) {
                while (!checkI2CStatusFlag(2));  // Wait for RXNE flag
                buffer[i] = I2C2->RXDR;
            }
        } else {
            for (int i = 0; i < count; i++) {
                while (!checkI2CStatusFlag(1));  // Wait for TXIS flag
                I2C2->TXDR = buffer[i];
            }
        }
    }
    
    // Wait until the transaction is complete
    while (!(I2C2->ISR & (1 << 6)));
    
    // End the I2C transaction by setting the STOP bit
    I2C2->CR2 |= (1 << 14);
}

// Checks the I2C status flag
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


int lab5_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config(); // Configure the system clock

    // Enable GPIOC clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable GPIOB clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock    

    // Enable I2C2 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

    // setting up led
    GPIOC->MODER &= ~(0b11 << (6 * 2)); // Clear bits for PC6
    GPIOC->MODER &= ~(0b11 << (7 * 2)); // Clear bits for PC7
    GPIOC->MODER &= ~(0b11 << (8 * 2)); // Clear bits for PC8
    GPIOC->MODER &= ~(0b11 << (9 * 2)); // Clear bits for PC9
    GPIOC->MODER |= (0b01 << (6 * 2)); // Set PC6 as output
    GPIOC->MODER |= (0b01 << (7 * 2)); // Set PC7 as output
    GPIOC->MODER |= (0b01 << (8 * 2)); // Set PC8 as output
    GPIOC->MODER |= (0b01 << (9 * 2)); // Set PC9 as output

    I2C2->CR1 |= I2C_CR1_PE;  // Ensure I2C2 is enabled

    // Configure PB11
    GPIOB->MODER &= ~(3 << (11 * 2));
    GPIOB->MODER |= (2 << (11 * 2));
    GPIOB->OTYPER |= (1 << 11);
    GPIOB->AFR[1] &= ~(0xF << ((11 - 8) * 4));
    GPIOB->AFR[1] |= (1 << ((11 - 8) * 4));

    // Configure PB13
    GPIOB->MODER &= ~(3 << (13 * 2));
    GPIOB->MODER |= (2 << (13 * 2));
    GPIOB->OTYPER |= (1 << 13);
    GPIOB->AFR[1] &= ~(0xF << ((13 - 8) * 4));
    GPIOB->AFR[1] |= (0x5 << ((13 - 8) * 4));

    // Configure PB14 as Output, Push-Pull, Set High
    GPIOB->MODER &= ~(3 << (14 * 2));
    GPIOB->MODER |= (1 << (14 * 2));
    GPIOB->OTYPER &= ~(1 << 14);
    GPIOB->ODR |= (1 << 14); // Set high

    // Configure PC0
    GPIOC->MODER &= ~(3 << (0 * 2));
    GPIOC->MODER |= (1 << (0 * 2));
    GPIOC->ODR |= (1 << 0); // Set high

    // Configure PB15 to input mode to avoid conflicts with PB11
    GPIOB->MODER &= ~(3 << (15 * 2));

    //  Set the parameters in the TIMINGR register to use 100 kHz standard-mode I2C.
    I2C2->TIMINGR = (1U << 28) | (0x4U << 20) | (0x2U << 16) | (0xFU << 8) | (0x13U);

    // Enable the I2C2 peripheral by setting the PE bit in the CR1 register.
    I2C2->CR1 |= (1 << 0);

    // Step 1: Write transaction
    int dummy; // Dummy variable
    gyroscope(0x69, 1, &dummy, 0, whoAmIReg);

    // Step 2: Read transaction
    int whoValue = 0;
    gyroscope(0x69, 1, &whoValue, 1, whoAmIReg);

    // Step 3: Compare the received value with the expected value (0xD4)
    if (whoValue == expectedWhoAmI) {
        // Toggle all leds if success
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); // Toggle RED LED
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); // Toggle BLUE LED
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); // Toggle ORANGE LED
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); // Toggle GREEN LED
    } else {
        // Turn on red led to indicate fail
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET); // Blue LED on
    }
    
    return 0;
    
}