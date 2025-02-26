#include <stm32f0xx_hal.h>
#include "main.h" // main header file

int lab4_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config(); // Configure the system clock

    // pins located near bottom-left ground pin
    // USART 3 TX pin = PB10
    // USART 3 RX pin = PB11

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable GPIOB clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enable USART 3 clock
    
    GPIOB->MODER &= ~(GPIO_MODER_MODER10); // clear mode bits for PB10
    GPIOB->MODER &= ~(GPIO_MODER_MODER11); // clear mode bits for PB11
    GPIOB->MODER |= (GPIO_MODER_MODER10_1); // set to AF mode
    GPIOB->MODER |= (GPIO_MODER_MODER11_1); // set to AF mode

    // Set AF4 (USART3) for PB10 and PB11
    GPIOB->AFR[1] |= (4 << GPIO_AFRH_AFSEL10_Pos);
    GPIOB->AFR[1] |= (4 << GPIO_AFRH_AFSEL11_Pos);

    USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200;  // set baud rate to 115200
    USART3->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; // Enable TX, RX, and USART

    // Configure PC6, PC7, PC8, and PC9 as output
    // PC6 = Red LED
    // PC7 = Blue LED
    // PC8 = Green LED
    // PC9 = Orange LED
    GPIOC->MODER &= ~(0b11 << (6 * 2)); // Clear bits for PC6
    GPIOC->MODER &= ~(0b11 << (7 * 2)); // Clear bits for PC7
    GPIOC->MODER &= ~(0b11 << (8 * 2)); // Clear bits for PC8
    GPIOC->MODER &= ~(0b11 << (9 * 2)); // Clear bits for PC9
    GPIOC->MODER |= (0b01 << (6 * 2)); // Set PC6 as output
    GPIOC->MODER |= (0b01 << (7 * 2)); // Set PC7 as output
    GPIOC->MODER |= (0b01 << (8 * 2)); // Set PC8 as output
    GPIOC->MODER |= (0b01 << (9 * 2)); // Set PC9 as output

    // Infinite while loop
    while (1)
    {
        if (USART3->ISR & USART_ISR_RXNE) {
            char c = USART3->RDR;
            switch (c) {
            // r key is pressed
            case 'r':
                transmit_string("RED LED\n");
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); // toggle red led
                break;
            
            case 'b':
                transmit_string("BLUE LED\n");
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); // toggle blue led
                break;

            case 'o':
                transmit_string("ORANGE LED\n");
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); // toggle orange led
                break;

            case 'g':
                transmit_string("GREEN LED\n");
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); // toggle green led
                break;


            
            // case for when a key other than the ones above is not pressed
            default:
                transmit_string("ERROR: invalid key\n");
                break;
            }   
        } // Check flags
    }
    
}

// additional functions for lab 4

// This function tramsmits a single character
void transmit_char(char c) {
    // infinite loop
    while (!(USART3->ISR & USART_ISR_TXE))  // exits once the flags are set
    {}
    USART3->TDR = c; // Write character to the transmit data register
    return;
}

// This function transmists a single string
void transmit_string(char string[]) {
    for(int i = 0; i < string[i] != '\0'; i++) {
        transmit_char(string[i]);
    }
    return;
}