#include <stm32f0xx_hal.h>
#include "main.h" // main header file

// This function preforms a blocking I2C transaction to read the WHO_AM_I register
uint8_t I2C_Read_WHO_AM_I(void) {
    // 1. Set the transaction parameters in the CR2 register. 
    I2C2->CR2 = ((0x6B << 1) | // Slave address (7-bit, shifted)
                  (1 << 16) | // NBYTES = 1 (starting at bit 16)
                  (0 << 10) | // RD_WRN = 0 for write
                  I2C_CR2_START); // Set START bit

    // 2. Wait until either of the TXIS (Transmit Register Empty/Ready) or 
    // NACKF (Slave NotAcknowledge) flags are set.
    while (!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)))
    {
        // will exit if TXIS or NACKF flags are set.
    }

    // If NACKF flag is set, clear the flag and return error code (0xFF)
    if (I2C2->ISR & I2C_ISR_NACKF) {
        I2C2->ICR = I2C_ICR_NACKCF; // Clear NACK flag
        return 0xFF; // Error: Slave did not acknowledge
    }
    // continue of the TXIS flag is set

    // 3. Write the address of the “WHO_AM_I” register into the I2C transmit register. (TXDR)
    I2C2->TXDR = 0x0F;

    // 4. Wait until the TC (Transfer Complete) flag is set
    while (!(I2C2->ISR & I2C_ISR_TC))
    {
        // Wait for TC flag
    }

    // 5. Configure CR2 for a read transaction
    // Set slave address (0x6B << 1), NBYTES = 1, RD_WRN = 1 (read), and set START for repeated start
    I2C2->CR2 = ((0x6B << 1) | // Slave address (7-bit, shifted)
                (1 << 16) | // NBYTES = 1
                I2C_CR2_RD_WRN | // RD_WRN = 1 for read
                I2C_CR2_START); // Set START bit to preform restart condition
    
    // 6. Wait until either of the RXNE (Receive Register Not Empty) or NACKF (Slave NotAcknowledge)
    // flags are set.
    while (!(I2C2->ISR & (I2C_ISR_RXNE | I2C_ISR_NACKF)))
    {
        // Wait for RXNE or NACKF
    }
    // If NACKF flag is set during read, clear flag and return error
    if (I2C2->ISR & I2C_ISR_NACKF) {
        I2C2->ICR = I2C_ICR_NACKCF; // Clear NACK flag
        return 0xFF;                // Error: Slave did not acknowledge read
    }

    // 7. Wait until the Transfer Complete (TC) flag is set
    while (!(I2C2->ISR & I2C_ISR_TC))
    {
        // Wait for TC flag
    }

    // 8. Check the contents of the RXDR register to see if it matches 0xD4. (expected value of the 
    // “WHO_AM_I” register)
    uint8_t who_am_i = I2C2->RXDR;

    // 9. Set the STOP bit in the CR2 register to release the I2C bus.
    I2C2->CR2 |= I2C_CR2_STOP;

    // Return the read WHO_AM_I value
    return who_am_i;
}

int lab5_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config(); // Configure the system clock

    // WHO AM I Register
    uint8_t who_am_i_val = I2C_Read_WHO_AM_I();

    // Enable GPIOC clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable GPIOB clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock    

    // Enable I2C2 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

    GPIO_InitTypeDef GPIO_InitStruct = {0}; // Configure GPIO Init structure

    // Configure PB11
    GPIO_InitStruct.Pin = GPIO_PIN_11; // Select PB11
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; // Alternate Function Open-Drain
    GPIO_InitStruct.Pull = GPIO_NOPULL; // No internal pull-up/down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // High-speed
    GPIO_InitStruct.Alternate = GPIO_AF1_I2C2; // Select AF1 (I2C2_SDA)

    // Apply configuration
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure PB13
    GPIO_InitStruct.Pin = GPIO_PIN_13; // Select PB13
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; // Alternate Function Open-Drain
    GPIO_InitStruct.Pull = GPIO_NOPULL; // No internal pull-up/down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // High-speed
    GPIO_InitStruct.Alternate = GPIO_AF1_I2C2; // Select AF1 (I2C2_SDA)

    // Apply configuration
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure PB15
    GPIO_InitStruct.Pin = GPIO_PIN_15; // Select PB15
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT; // Set as input
    GPIO_InitStruct.Pull  = GPIO_NOPULL;  // No internal pull-up/down
    
    // Apply configuration
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure PB14 as Output, Push-Pull, Set High
    GPIO_InitStruct.Pin = GPIO_PIN_14; // Select PB14
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Output mode, Push-Pull
    GPIO_InitStruct.Pull = GPIO_NOPULL; // No internal pull-up/down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Low speed to reduce EMI
    
    // Apply configuration
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); // Set PB14 High

    // Disable I2C2 peripheral before configuring its registers.
    I2C2->CR1 &= ~I2C_CR1_PE;

    //  Set the parameters in the TIMINGR register to use 100 kHz standard-mode I2C.
    I2C2->TIMINGR = (0U << 28)   |   // PRESC
                     (4U << 20)   |   // SCLDEL
                     (2U << 16)   |   // SDADEL
                     (15U << 8)   |   // SCLH
                     (21U);           // SCLL

    // Enable the I2C2 peripheral by setting the PE bit in the CR1 register.
    I2C2->CR1 |= I2C_CR1_PE;

    // Check if the read value matches the expected WHO_AM_I value (0xD4)
    if (who_am_i_val == 0xD4) {
        // The WHO_AM_I register contains the expected value.
    }
    else {
        // Unexpected value received (or communication error).
    }

    // Infinite While Loop
    while(1) { }

    return 0;
    
}