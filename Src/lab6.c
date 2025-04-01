#include "main.h"
#include "hal_gpio.h"
#include <stm32f0xx_hal.h>

void SystemClock_Config(void);

int lab6_main(void) {
    HAL_Init();

    // Enable clock to GPIOC and GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN;

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable ADC1 clock

    ADC1->CFGR1 &= ~ADC_CFGR1_RES; // 8-bit resolution (00)
    ADC1->CFGR1 |= ADC_CFGR1_CONT; // Continuous conversion
    ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN; // Software trigger

    ADC1->CHSELR = ADC_CHSELR_CHSEL0; // Select channel 0 (PA0)

    ADC1->CR |= ADC_CR_ADCAL; // Start calibration
    while (ADC1->CR & ADC_CR_ADCAL); // Wait until calibration done

    ADC1->CR |= ADC_CR_ADEN; // Enable ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait for ADC ready

    ADC1->CR |= ADC_CR_ADSTART;

    // Set LEDs (PC6-PC9)
    GPIOC->MODER &= ~((3 << (2 * 6)) | (3 << (2 * 7)) | (3 << (2 * 8)) | (3 << (2 * 9))); // Clear first
    GPIOC->MODER |=  ((1 << (2 * 6)) | (1 << (2 * 7)) | (1 << (2 * 8)) | (1 << (2 * 9))); // Set as output

    GPIOA->MODER |= (3 << (2 * 0)); // Set PA0 to analog mode (11)
    GPIOA->PUPDR &= ~(3 << (2 * 0)); // No pull-up/pull-down

    // Infinite While Loop
    while(1) {
        // Wait until ADC conversion is complete
        if (ADC1->ISR & ADC_ISR_EOC) {
            uint8_t adc_value = ADC1->DR; // Read 8-bit ADC value

            // Clear all LED bits first (PC6 to PC9)
            GPIOC->ODR &= ~((0xF) << 6); // Clear PC6-PC9

            // Set LEDs based on increasing ADC thresholds
            if (adc_value >= 64)   GPIOC->ODR |= (1 << 6); // Turn on Red (PC6)
            if (adc_value >= 128)  GPIOC->ODR |= (1 << 7); // Turn on Blue (PC7)
            if (adc_value >= 192)  GPIOC->ODR |= (1 << 8); // Turn on Orange (PC8)
            if (adc_value >= 224)  GPIOC->ODR |= (1 << 9); // Turn on Green (PC9)
        }
    }
}