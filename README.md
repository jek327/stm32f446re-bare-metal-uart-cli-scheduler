# STM32 Bare-Metal UART CLI Scheduler

A bare-metal command-line interface (CLI) for the STM32F446RE built entirely through direct register manipulation without using the STM32 HAL.

This project combines a reusable UART driver, an interrupt-driven SysTick timer, and a cooperative superloop scheduler to create a responsive embedded application capable of executing commands while simultaneously running background tasks.

---

# Features

- Bare-metal USART2 configuration
- Full-duplex UART communication
- Polling-based UART transmit/receive driver
- Non-blocking UART receive interface
- Interrupt-driven SysTick timer (1 ms tick)
- Software millisecond counter
- Cooperative superloop scheduler
- Non-blocking LED blinking
- Fixed-length command buffer
- Command parsing using `strcmp()`
- Buffer overflow detection
- Modular driver architecture

---

# Hardware

- STM32 Nucleo-F446RE
- STM32F446RE (ARM Cortex-M4)
- ST-LINK Virtual COM Port
- USB connection to host computer

---

# Software

- STM32CubeIDE
- Embedded C
- CMSIS
- macOS Terminal (`screen`)

---

# Project Architecture

```
                 Application (main.c)
                         │
        ┌────────────────┼────────────────┐
        │                │                │
        ▼                ▼                ▼
   UART Driver      SysTick Driver    GPIO Driver
        │                │
        │                ▼
        │        1 ms Software Clock
        │                │
        └────────────┬───┘
                     ▼
         Cooperative Scheduler
```

---

# UART Driver

The UART driver exposes a reusable API.

```c
void UART2_Init(void);

void UART2_WriteChar(char c);

void UART2_WriteString(const char *message);

char UART2_ReadChar(void);

int UART2_CharAvailable(void);
```

---

# SysTick Driver

The SysTick driver provides a reusable software timing interface.

```c
void SysTick_Init(void);

uint32_t SysTick_GetCurrentTime(void);
```

A 1 ms interrupt updates an internal software timer:

```c
static volatile uint32_t millis;
```

The timer is encapsulated inside the SysTick driver and cannot be modified directly by the application.

---

# Cooperative Scheduler

The application continuously executes two independent tasks:

1. Background LED scheduler
2. UART command processing

```text
while (1)

    Update LED Scheduler

    Check UART

    Process Commands

repeat forever
```

Unlike blocking delays, both tasks continue running concurrently.

---

# Supported Commands

| Command | Description |
|---------|-------------|
| help | Display available commands |
| led on | Turn LED on |
| led off | Turn LED off |
| blink on | Enable scheduled LED blinking |
| blink off | Disable blinking |

---

# Non-Blocking Timing

The LED scheduler uses elapsed-time comparisons rather than blocking delays.

```c
if ((currentTime - lastBlinkTime) >= blinkIntervalMs)
{
    GPIOA->ODR ^= (1U << 5);
    lastBlinkTime = currentTime;
}
```

This allows the CLI to remain fully responsive while the LED blinks.

---

# UART Configuration

| Setting | Value |
|----------|------:|
| Peripheral | USART2 |
| Baud Rate | 115200 |
| Data Bits | 8 |
| Stop Bits | 1 |
| Parity | None |
| Flow Control | None |

---

# GPIO Configuration

| Pin | Function |
|-----|----------|
| PA2 | USART2_TX |
| PA3 | USART2_RX |
| PA5 | User LED |

---

# macOS Terminal

Find the serial device:

```bash
ls /dev/cu.usb*
```

Example:

```bash
/dev/cu.usbmodem143303
```

Connect:

```bash
screen /dev/cu.usbmodem143303 115200
```

Exit:

```
Ctrl-A
Ctrl-\
y
```

Terminate an existing session:

```bash
screen -ls

screen -S <session-id> -X quit
```

---

# Concepts Demonstrated

- Bare-metal embedded programming
- STM32 peripheral configuration
- Register-level programming
- UART communication
- Interrupt handling
- SysTick timer
- Software timers
- Cooperative multitasking
- Non-blocking programming
- Driver abstraction
- Module encapsulation
- Command-line interface (CLI)
- Embedded software architecture

---

# Future Improvements

- UART receive interrupts
- Ring buffer
- Backspace/Delete support
- Command history
- Configurable blink intervals
- Multiple software timers
- Button debouncing
- Task scheduler
- FreeRTOS migration

---

## 🎥 Demo Video

Watch the complete project demonstration:

[https://youtu.be/XXXXXXXXXXX](https://www.youtube.com/watch?v=KVTUMiLma2Y)

---

# Author

**Vasanthavel Jeeva Kumararaja**

GitHub: https://github.com/jek327
