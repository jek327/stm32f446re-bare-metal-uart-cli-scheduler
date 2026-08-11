/**
 ******************************************************************************
 * Project : STM32 Bare-Metal UART Command-Line Interface with
 *           Interrupt-Driven SysTick Scheduler
 * File    : main.c
 * Author  : Vasanthavel Jeeva Kumararaja
 * Date    : August 11, 2026
 *
 * Description:
 * Demonstrates a bare-metal command-line interface (CLI) on the
 * STM32F446RE using USART2 for full-duplex serial communication.
 * All peripheral configuration is performed through direct register
 * manipulation without using the STM32 HAL APIs.
 *
 * GPIOA Pin 2 is configured as USART2_TX and GPIOA Pin 3 as USART2_RX
 * using Alternate Function 7 (AF7) to communicate with a host computer
 * through the on-board ST-LINK Virtual COM Port.
 *
 * The application implements a non-blocking serial command-line
 * interface capable of receiving, buffering, parsing, and executing
 * user commands entered through a terminal. Incoming characters are
 * processed using a reusable UART driver with polling-based reception,
 * while system timing is provided by an interrupt-driven SysTick driver.
 *
 * A 1 ms SysTick interrupt maintains a continuously increasing software
 * timer, allowing the application to perform non-blocking scheduling.
 * This timer is used to implement responsive LED blinking while
 * simultaneously accepting UART commands, demonstrating a cooperative
 * superloop architecture.
 *
 * Learning Objectives:
 * - Configure USART2 for full-duplex communication using bare-metal programming
 * - Enable GPIOA and USART2 peripheral clocks through the RCC
 * - Configure GPIOA Pins 2 and 3 for Alternate Function 7 (USART2)
 * - Configure the UART baud rate using the Baud Rate Register (BRR)
 * - Enable the USART transmitter and receiver through Control Register 1 (CR1)
 * - Implement reusable UART driver functions for transmit and receive
 * - Poll the TXE and RXNE status flags for UART communication
 * - Build a non-blocking UART receive interface using UART2_CharAvailable()
 * - Store incoming characters in a fixed-length command buffer
 * - Parse user commands using strcmp()
 * - Detect and handle command buffer overflow
 * - Configure SysTick to generate a 1 ms interrupt
 * - Maintain a software millisecond counter using SysTick_Handler()
 * - Build a reusable SysTick timing driver
 * - Implement non-blocking software timers using elapsed-time comparisons
 * - Control hardware through a serial command-line interface
 * - Design a cooperative superloop scheduler without an RTOS
 *
 * Supported Commands:
 * - help
 * - led on
 * - led off
 * - blink on
 * - blink off
 *
 * Current Limitations:
 * - UART communication uses polling instead of receive interrupts
 * - Command buffer is limited to 19 characters
 * - Command editing (Backspace/Delete) is not implemented
 * - Commands are case-sensitive
 * - Command history and auto-completion are not implemented
 * - Single software timer implementation
 * - Fixed LED blink interval (500 ms)
 *
 * GitHub:
 * github.com/jek327
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "uart.h"
#include "string.h"
#include "systick.h"


void GPIO_init(unsigned int clockEnable, unsigned int ioModeClear, unsigned int ioMode);

int main(void)
{
	// initialize USART2
	UART2_Init();

	// initialize User LED
	GPIO_init(0x00000001, 0x00000C00, 0x00000400);

	// initialize systick (interrupt and counter initialization)
	SysTick_Init();

	// blinking state
	int blinking = 0;

	// time when last blinking occurred
	uint32_t lastBlinkTime = 0;

	// time interval we want LED to blink
	uint32_t blinkIntervalMs = 500;

	// buffer
	char buffer[20];
	int index = 0;
	char c;

	/* Infinite loop */
	while (1)
	{
		// checks if blinking is selected
		if (blinking == 1)
		{
			// sets currentTime using Systick function
			uint32_t currentTime = SysTick_GetCurrentTime();

			// checks if the elapsed time is greater than the desired interval
			if (currentTime - lastBlinkTime >= blinkIntervalMs)
			{
				// toggles LED
				GPIOA->ODR ^= 0x00000020;

				// sets the blinkTime to the latest time
				lastBlinkTime = currentTime;
			}
		}

		// checks whether a char is ready to be read
		if (UART2_CharAvailable())
		{
			// reads the char value into c
			c = UART2_ReadChar();

			// checks buffer overflow
			if (index > 18)
			{
				// will print message and reset index only if user presses enter
				if (c == '\r')
				{
					UART2_WriteString("\r\nToo Many Characters!\r\n");
					index = 0;
				}
				else
				{
					// simply prints character without updating buffer
					UART2_WriteChar(c);
				}
			}
			// checks if user pressed enter
			else if (c == '\r')
			{
				// sets current index to null terminator
				buffer[index] = '\0';

				// checks if stored buffer is 'help'
				if (strcmp(buffer, "help") == 0)
				{

					UART2_WriteString("\r\nAvailable commands: help, led on, led off, blink on, blink off\r\n");
				}
				else if (strcmp(buffer, "led on") == 0)
				{
					UART2_WriteString("\r\nLED ON\r\n");
					blinking = 0;
					GPIOA->ODR |= 0x00000020;

				}
				else if (strcmp(buffer, "led off") == 0)
				{
					UART2_WriteString("\r\nLED OFF\r\n");
					blinking = 0;
					GPIOA->ODR &= ~(0x00000020);
				}
				else if (strcmp(buffer, "blink on") == 0)
				{
					UART2_WriteString("\r\nBLINK ON\r\n");
					blinking = 1;
					lastBlinkTime = SysTick_GetCurrentTime();
				}
				else if (strcmp(buffer, "blink off") == 0)
				{
					UART2_WriteString("\r\nBLINK OFF\r\n");
					blinking = 0;
					GPIOA->ODR &= ~(0x00000020);
				}
				else
				{
					UART2_WriteString("\r\nUnknown command. Type 'help'\r\n");
				}
				index = 0;
			}
			else
			{
				buffer[index] = c;
				UART2_WriteChar(c);
				index++;
			}
		}

	}
}


/**
 * unsigned int clockEnable -- register value to enable clock
 * unsigned int ioModeClear -- register value to clear specified Mode register bits
 * unsigned int ioMode      -- register value to set Mode register bits
**/
void GPIO_init(unsigned int clockEnable, unsigned int ioModeClear, unsigned int ioMode)
{
	 // Enables clock register for GPIOA peripheral
	  RCC->AHB1ENR |= clockEnable;

	  // Clears I/O direction mode register
	  GPIOA->MODER &= ~(ioModeClear);

	  // Configures I/O direction mode for Port
	  GPIOA->MODER |= ioMode;
}

/******************************************************************************
 * UART Serial Terminal (macOS)
 * ----------------------------------------------------------------------------
 * 1. Find the Virtual COM Port
 *
 *    ls /dev/cu.usb*
 *
 *    Example Output:
 *    /dev/cu.usbmodem143303
 *
 * ----------------------------------------------------------------------------
 * 2. Connect to the STM32 UART
 *
 *    screen /dev/cu.usbmodem143303 115200
 *
 *    UART Configuration:
 *      - Baud Rate : 115200
 *      - Data Bits : 8
 *      - Parity    : None
 *      - Stop Bits : 1
 *      - Flow Ctrl : None
 *
 * ----------------------------------------------------------------------------
 * 3. Exit Screen
 *
 *    Press:
 *
 *      Ctrl-A
 *      Ctrl-\
 *
 *    Then press:
 *
 *      y
 *
 * ----------------------------------------------------------------------------
 * 4. If "Sorry, could not find a PTY." Appears
 *
 *    List active screen sessions:
 *
 *      screen -ls
 *
 *    Terminate the old session:
 *
 *      screen -S <session-id> -X quit
 *
 *    Example:
 *
 *      screen -S 31395.ttys001.Jeevas-MBP -X quit
 *
 *    Reconnect:
 *
 *      screen /dev/cu.usbmodem143303 115200
 *
 * ----------------------------------------------------------------------------
 * UART Data Path
 *
 *      STM32 USART2
 *            │
 *            ▼
 *         PA2 (TX)
 *            │
 *            ▼
 *      ST-LINK Virtual COM Port
 *            │
 *            ▼
 *           USB
 *            │
 *            ▼
 *   macOS (/dev/cu.usbmodemXXXXX)
 *            │
 *            ▼
 *      Terminal (screen)
 *
 ******************************************************************************/
