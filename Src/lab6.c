#include "main.h"
#include "hal_gpio.h"
#include <stm32f0xx_hal.h>

void SystemClock_Config(void);
    
int lab6_main(void) {
    HAL_Init();

    // Enable clock to GPIOC and GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN;

    // Enable ADC1 clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // Enable APB1 clock
    RCC->APB1ENR |= RCC_APB1ENR_DACEN; 

    // Set LEDs (PC6-PC9)
    GPIOC->MODER &= ~((3 << (2 * 6)) | (3 << (2 * 7)) | (3 << (2 * 8)) | (3 << (2 * 9))); // Clear first
    GPIOC->MODER |=  ((1 << (2 * 6)) | (1 << (2 * 7)) | (1 << (2 * 8)) | (1 << (2 * 9))); // Set as output

    ADC1->CFGR1 &= ~ADC_CFGR1_RES; // 8-bit resolution (00)
    ADC1->CFGR1 |= ADC_CFGR1_CONT; // Continuous conversion
    ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN; // Software trigger

    ADC1->CHSELR = ADC_CHSELR_CHSEL0; // Select channel 0 (PA0) for ADC Input

    GPIOA->MODER |= (3 << (2 * 0)); // Set PA0 to analog mode
    GPIOA->PUPDR &= ~(3 << (2 * 0)); // No pull-up/pull-down

    GPIOA->MODER |= (3 << 8);  // Set PA4 to analog mode
    GPIOA->PUPDR &= ~(3 << 8); // No pull-up/pull-down

    // setting up PC0
	GPIOC->MODER |= (1 << 0);
	GPIOC->PUPDR &= ~(1 << 0);

    // Set up DAC channel
    DAC1->CR &= ~(DAC_CR_TEN1 | DAC_CR_TSEL1); // Disable trigger mode
    DAC1->CR |= DAC_CR_EN1; // Enable DAC channel 1

    // Start Calibration
    // Ensure ADEN is 0
    if((ADC1->CR & ADC_CR_ADDIS) != 0) {
        ADC1->CR |= ADC_CR_ADDIS; // Clear ADEN by setting ADDIS
    }
    while ((ADC1->CR & ADC_CR_ADDIS) != 0) {}
    ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN; // Clear DMAEN
    ADC1->CR |= ADC_CR_ADCAL; // Launch Calibration
    // Wait until ADCAL = 0
    while ((ADC1->CR & ADC_CR_ADCAL) != 0) {}

    // Start Enable
    // Ensure ADRDY is 0
    if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) {
        ADC1->ISR |= ADC_ISR_ADRDY; // Clear ADRDY
    }
    ADC1->CR |= ADC_CR_ADEN; // Launch Enable
    // Wait until ADRDY = 0
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {}    

    ADC1->CR |= ADC_CR_ADSTART;

    // Sine Wave: 8-bit, 32 samples/cycle
    const uint8_t sine_table[32] = {127,151,175,197,216,232,244,251,254,251,244,
        232,216,197,175,151,127,102,78,56,37,21,9,2,0,2,9,21,37,56,78,102};
    

    int i = 0;
    // Infinite While Loop
    while(1) {
        uint8_t adc_value = ADC1->DR; // Read 8-bit ADC value

        // Clear all LED bits first (PC6 to PC9)
        GPIOC->ODR &= ~((0xF) << 6); // Clear PC6-PC9

        // Set LEDs based on increasing ADC thresholds (note the potentimeter that I am using is a 104)
        // Also, the leds will cycle back to "0" for some reason, could be the potentimeter
        if (adc_value > 64) My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); // RED LED on
        if (adc_value > 128) My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET); // BLUE LED on
        if (adc_value > 192) My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // ORANGE LED on
        if (adc_value > 224) My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // GREEN LED on

        DAC1->DHR8R1 = sine_table[i]; // Write 8-bit value to DAC channel 1
        i = (i + 1) % 32;
        HAL_Delay(1); // 1ms delay
    }
}