#include <stm32f0xx_hal.h>
#include "main.h" // main header file
int lab4_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config(); // Configure the system clock

    // pins located near bottom-left ground pin
    // USART 3 TX pin = PB10
    // USART 3 RX pin = PB11

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable GPIOB clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enable USART 3 clock

    GPIOB->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11);  // Clear bits
    GPIOB->MODER |= (GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1);  // Set to AF mode

    USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200;  // set baud rate to 115200
    USART3->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; // Enable TX, RX, and USART

    // Infinite while loop
    while (1)
    {
    }
    
}