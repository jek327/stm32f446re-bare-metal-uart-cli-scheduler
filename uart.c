#include "uart.h"
#include "stm32f446xx.h"


void UART2_Init(void)
{
	// enable GPIOA clock
	RCC->AHB1ENR |= 0x00000001U;

	// enable USART2 clock
	RCC->APB1ENR |= 0x00020000U;

	// clear Alternate Function Mode field for PA2 and PA3
	GPIOA->MODER &= ~(0x000000F0U);

	// set GPIOA Pin 2 and Pin 3 to Alternate Function Mode
	GPIOA->MODER |= 0x000000A0U;

	// clear Alternate Function low register bit for pin 2/3
	GPIOA->AFR[0] &= ~(0x0000FF00U);

	// set PA2 and PA3 to AF7
	GPIOA->AFR[0] |= (0x00007700U);

	// Set mantissa and fraction for Baud rate
	USART2->BRR = 0x0000008BU;

	// enable USART, Transmitter and Receiver
	USART2->CR1 = 0x0000200CU;
}

/**
 * char c -- the individual character we want to transmit
 */
void UART2_WriteChar(char c)
{
	// Waits until the transmit data register is empty
	while ((USART2->SR & (1U << 7)) == 0U)
	{}
	// Load the next character into the data register
	USART2->DR = c;
}

/**
 *  const char *message -- takes a pointer to array to transmit each char of a string
 */
void UART2_WriteString(const char *message)
{
	// writes until it reaches the null terminator
	while (*message != '\0')
	{
		// Use UART2_WriteChar() to transmit each character
		UART2_WriteChar(*message);
		// iterates by 1 to traverse the whole array
		message++;
	}
}

/*
 * Non-blocking implementation: Must use in conjunction with CharAvailable function
 */
char UART2_ReadChar(void)
{
	// we return the char value being read
	return (char)(USART2->DR & 0xFFU);
}

/*
 * checks whether RXNE bit is on or ready to read data (use with ReadChar)
 */
int UART2_CharAvailable(void)
{
	return ((USART2->SR & (1U << 5)));
}


