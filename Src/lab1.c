#include <stm32f0xx_hal.h>
int lab1_main(void) {
    // HAL_Init(); // Reset of all peripherals, init the Flash and Systick
    // SystemClock_Config(); //Configure the system clock
    // /* This example uses HAL library calls to control
    // the GPIOC peripheral. You’ll be redoing this code
    // with hardware register access. */
    // __HAL_RCC_GPIOC_CLK_ENABLE(); // Enable the GPIOC clock in the RCC
    // // Set up a configuration struct to pass to the initialization function
    // GPIO_InitTypeDef initStr = {GPIO_PIN_8 | GPIO_PIN_9,
    // GPIO_MODE_OUTPUT_PP,
    // GPIO_SPEED_FREQ_LOW,
    // GPIO_NOPULL};
    // HAL_GPIO_Init(GPIOC, &initStr); // Initialize pins PC8 & PC9
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // Start PC8 high
    // while (1) {
    //     HAL_Delay(200); // Delay 200ms
    //     // Toggle the output state of both PC8 and PC9
    //     HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    // }
    
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config();  // Configure the system clock

    // Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Configure PC6, PC7 as output mode
    GPIOC->MODER &= ~(3 << (6 * 2)); // Clear mode bits for PC6
    GPIOC->MODER &= ~(3 << (7 * 2)); // Clear mode bits for PC7
    GPIOC->MODER |= (1 << (6 * 2));  // Set PC6 as the output
    GPIOC->MODER |= (1 << (7 * 2));  // Set PC7 as the output

    // Set PC6 ON and PC7 OFF
    GPIOC->BSRR = (1 << 6);  // Set PC6 (turn on red LED)
    GPIOC->BSRR = (1 << (7 + 16));  // Reset PC7 (turn off blue LED)
    
    // Infinite While Loop
    while (1) {
        HAL_Delay(200);  // 200ms Delay
        GPIOC->ODR ^= (1 << 6) | (1 << 7); // Toggle PC6 and PC7
    }
}