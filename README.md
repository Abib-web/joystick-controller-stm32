# Joystick-Controlled Vehicle Project with STM32

This project demonstrates the integration of a joystick with an STM32 microcontroller for controlling a vehicle. Using I2C communication, the joystick is used to manage directional movements and speed adjustments. The system also includes real-time feedback displayed on an LCD.

---

## Features

### Joystick Integration:
- The joystick communicates via the I2C protocol.
- It controls forward, reverse, left, and right movements.
- Buttons on the joystick are used for additional functionalities like mode toggling.

### Real-Time Feedback:
- An LCD displays the current state of the vehicle, including the speed and mode (manual or automatic).
- The Real-Time Clock (RTC) provides time and date on the display.

### Acceleration Data:
- The accelerometer embedded in the joystick provides pitch and roll data for advanced control.

### I2C Protocol Implementation:
- Communication between the joystick and the microcontroller uses the I2C protocol.
- Addressing, data transmission, and acknowledgment bits are handled by HAL functions.

---

## Getting Started

### Hardware Requirements
- STM32F446RE development board.
- Wii Nunchuk joystick or any compatible I2C joystick.
- LCD module for real-time data display.
- Pull-up resistors (4.6kΩ) for the I2C lines.
- 3.3V power supply for the joystick.

### Software Requirements
- STM32CubeMX for project configuration.
- STM32CubeIDE for coding and debugging.
- HAL library for I2C and LCD management.

---

## Configuration

### Joystick Connection

**Pin Configuration:**
- **SDA**: PB9
- **SCL**: PB8
- Power and ground connected to 3.3V and GND pins of the Nucleo board.

**Pull-up Resistors:**
- Connect 4.6kΩ resistors between SDA, SCL, and the 3.3V line.

**Initialization:**
- Configure the I2C protocol in standard mode with a 7-bit address.

### LCD Configuration
- Use Lab #6 code as a base to interface the LCD.
- Display data including joystick input and real-time clock information.

---

## Example Code

```c
// I2C Address for the joystick
#define NUNCHUK_ADDRESS 0x52 << 1

// Data array for receiving joystick data
uint8_t data[6];

// Initialize the joystick
HAL_I2C_Master_Transmit(&hi2c1, NUNCHUK_ADDRESS, init, 2, HAL_MAX_DELAY);

// Receive joystick data
HAL_I2C_Master_Receive(&hi2c1, NUNCHUK_ADDRESS, data, 6, HAL_MAX_DELAY);

// Parse joystick data
int8_t joystick_x = data[0];
int8_t joystick_y = data[1];
``
