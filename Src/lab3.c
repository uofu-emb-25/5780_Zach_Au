#include <stm32f0xx_hal.h>
#include "main.h" // main header file
int lab3_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick

    // Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Configure PC6, PC7, PC8, and PC9 as output
    // PC6 = Red LED
    // PC7 = Blue LED
    // PC8 = Green LED
    // PC9 = Orange LED
    GPIOC->MODER &= ~(0b11 << (8 * 2)); // Clear bits for PC8
    GPIOC->MODER &= ~(0b11 << (9 * 2)); // Clear bits for PC9
    GPIOC->MODER |= (0b01 << (8 * 2)); // Set PC8 as output
    GPIOC->MODER |= (0b01 << (9 * 2)); // Set PC9 as output

    // Enable TIM2 peripheral clock
    RCC->APB1ENR |= (1 << 0);  // Set TIM2EN bit in RCC_APB1ENR

    TIM2->PSC = 7999; // Set PSC to 8000 = 8 Mhz / 8000 = 1 kHz
    TIM2->ARR = 250; // Set ARR to 250 = 1 kHz / 250 = 4 Hz
    TIM2->EGR |= TIM_EGR_UG; // Enable Update Event (UEV)
    TIM2->CR1 |= TIM_CR1_CEN; // Enable Timer Counter
    TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt for TIM2
    TIM2->CR1 |= TIM_CR1_CEN; // Enable Timer Counter

    // Enable TIM2 interrupt in NVIC
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 2);  // Set priority (lower is higher priority)

    // Initial LED setup
    GPIOC->BSRR = (1 << 8); // PC8 (Green) is on
    GPIOC->BSRR = (1 << (9 + 16)); // PC9 (Orange) is off

    GPIO_Init_TIM3_PWM();

    TIM3_PWM();

    // Empty infinite loop, interupt should handle everything
    while (1) {}
}