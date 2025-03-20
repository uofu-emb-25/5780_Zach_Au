#include <stm32f0xx_hal.h>
#include "main.h" // main header file

int lab5_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config(); // Configure the system clock


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
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2; // Select AF4 (I2C2_SDA)

    // Apply configuration
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure PB13
    GPIO_InitStruct.Pin = GPIO_PIN_13 // Select PB13
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; // Alternate Function Open-Drain
    GPIO_InitStruct.Pull = GPIO_NOPULL; // No internal pull-up/down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // High-speed
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2; // Select AF4 (I2C2_SDA)

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

    return 0;
    
}