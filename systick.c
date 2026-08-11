#include "systick.h"
#include "stm32f446xx.h"

#define SYSTICK_COUNTFLAG (1U << 16)
#define SYSTICK_ENABLE (1U << 0)
#define SYSTICK_TICKINT (1U << 1)
#define SYSTICK_CLKSOURCE (1U << 2)

// timer counter private to systick.c
static volatile uint32_t millis;

// Systick initialization
void SysTick_Init(void)
{
	// initialize counter to 0
	millis = 0;

	// Disables systick while trying to configure it
	SysTick->CTRL = 0U;

	// sets reload value to exactly 1 ms (we want timer in ms not s)
	SysTick->LOAD = (16000000U / 1000U) -1U;

	// resets the counter value to 0
	SysTick->VAL = 0U;

	// sets clock source to processor clock, enables systick interrupt and counter
	SysTick->CTRL = SYSTICK_ENABLE | SYSTICK_TICKINT | SYSTICK_CLKSOURCE;

}

// Systick delay function in milliseconds
void SysTick_DelayMs(uint32_t ms)
{
	//looping the number of milliseconds
	for (uint32_t i = 0; i < ms; i++)
	{
		// waits until COUNTFLAG is set to 1
		while ((SysTick->CTRL & SYSTICK_COUNTFLAG) == 0U)
		{}
	}
}

// Interrupt handler which increments timer counter
void SysTick_Handler(void)
{
	millis++;
}

// Returns the current time
uint32_t SysTick_GetCurrentTime(void)
{
	return millis;
}


