#include <stm32f0xx_hal.h>
#include <assert.h>  // Include assert.h for assertions
int lab2_main(void) {
    HAL_Init();  // Reset all peripherals, initialize Flash and Systick
    SystemClock_Config();  // Configure the system clock

    // Enable GPIOC clock in RCC
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Check if GPIOC clock is enabled in RCC
    assert((RCC->AHBENR & RCC_AHBENR_GPIOCEN) != 0);

    // Configure GPIOC 6 & 9
    GPIO_InitTypeDef initStr = {
        GPIO_PIN_6 | GPIO_PIN_9,
        GPIO_MODE_OUTPUT_PP,
        GPIO_SPEED_FREQ_LOW,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOC, &initStr);

    // Check if GPIOC->MODER is correctly set to output mode
    assert((GPIOC->MODER & (0b01 << (6 * 2))) != 0);  // PC6, Red LED
    assert((GPIOC->MODER & (0b01 << (9 * 2))) != 0);  // PC9, Green LED

    // Check if GPIOC->PUPDR is set to no pull-up/pull-down
    assert((GPIOC->PUPDR & (0b00 << (6 * 2))) != 0);
    assert((GPIOC->PUPDR & (0b00 << (9 * 2))) != 0);

    // Set PC9 (green LED) to high
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

    // Check if GPIOC->ODR is correctly set for PC8 to high
    assert((GPIOC->ODR & GPIO_PIN_6) != 0);  // PC6 should be high

    // Infinite Loop
    while(1) {
        HAL_Delay(200); // delay for 200 ms
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_9);

        // Ensure GPIO state toggles correctly
        assert((GPIOC->ODR & (GPIO_PIN_6 | GPIO_PIN_9)) == 0 || 
            (GPIOC->ODR & (GPIO_PIN_6 | GPIO_PIN_9)) == (GPIO_PIN_6 | GPIO_PIN_9));
    }

}