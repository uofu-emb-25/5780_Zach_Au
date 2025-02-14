#include <stm32f0xx_hal.h>
#include <assert.h> // for assert function
#include "hal_gpio.h" // header file
#include "main.h" // main header file

int lab2_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config();  // Configure the system clock

    // Enable GPIOC clock in RCC
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure GPIOC 6, 7, 8, 9
    GPIO_InitTypeDef initStr = {
        GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
        GPIO_MODE_OUTPUT_PP,
        GPIO_SPEED_FREQ_LOW,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOC, &initStr);

    // Set PC9 (Green LED) to high
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

    // EXTI line 0 needs to be masked and trigger not enabled
    assert((EXTI->IMR & (1 << 0)) == 0);
    assert((EXTI->RTSR & (1 << 0)) == 0);

    // Call function to configure EXTI for PA0.
    Configure_EXTI_UserButton();

    assert((EXTI->IMR & (1 << 0)) != 0);
    assert((EXTI->RTSR & (1 << 0)) != 0);

    // NVIC configuration: Enable EXTI0_1 interrupt
    NVIC_SetPriority(EXTI0_1_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_1_IRQn);

    // Start with red LED on and blue LED off.
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); // Red LED on
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); // Blue LED off

    // Infinite Loop
    while(1) {
        HAL_Delay(500); // delay for 500 ms
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); // Red LED
    }
}