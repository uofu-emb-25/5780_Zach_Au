#include <stm32f0xx_hal.h>
int lab2_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config();  // Configure the system clock

    // Enable GPIOC clock in RCC
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure GPIOC 6 & 9
    GPIO_InitTypeDef initStr = {
        GPIO_PIN_6 | GPIO_PIN_9,
        GPIO_MODE_OUTPUT_PP,
        GPIO_SPEED_FREQ_LOW,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOC, &initStr);

    // Set PC9 (green LED) to high
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

    // Infinite Loop
    while(1) {
        HAL_Delay(500); // delay for 500 ms
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); // Red LED
    }

}