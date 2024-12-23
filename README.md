# Joystick-Controlled Vehicle Project with STM32

This project demonstrates the integration of a joystick with an STM32 microcontroller for controlling a vehicle. Using I2C communication, the joystick is used to manage directional movements and speed adjustments. The system also includes real-time feedback displayed on a UART terminal.

---

## Features

### Joystick Integration:
- The joystick communicates via the I2C protocol.
- It controls forward, reverse, left, and right movements.
- Buttons on the joystick are used for additional functionalities like mode toggling.

### Real-Time Feedback:
- Data from the joystick is displayed in real-time on a UART terminal.
- The output includes joystick position, acceleration data, and button states.

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
- UART-to-USB converter or similar for terminal communication.
- Pull-up resistors (4.6kΩ) for the I2C lines.
- 3.3V power supply for the joystick.

### Software Requirements
- STM32CubeMX for project configuration.
- STM32CubeIDE for coding and debugging.
- PuTTY or Tera Term for UART terminal output.
- HAL library for I2C and UART management.

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

### UART Configuration
- Set up UART in STM32CubeMX for serial communication.
- Use a baud rate of 9600 or higher for data output.

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
int accel_x = (data[2] << 2) | ((data[5] >> 2) & 0x03);
int accel_y = (data[3] << 2) | ((data[5] >> 4) & 0x03);
int accel_z = (data[4] << 2) | ((data[5] >> 6) & 0x03);

// Transmit parsed data via UART
char buffer[100];
sprintf(buffer, "Joystick X: %d, Y: %d\nAccel X: %d, Y: %d, Z: %d\nButton C: %d, Button Z: %d\n",
        joystick_x, joystick_y, accel_x, accel_y, accel_z, 
        !(data[5] & 0x02), !(data[5] & 0x01));
HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
```

## Video Demonstration

- [Joystick Manipulation and Vehicle Control](https://drive.google.com/file/d/1689lBxDMI8c7FaeYoWoYacifNVS20Pdw/view?usp=sharing)

---

## Images

### UART Terminal Output:
![UART Terminal Output](https://github.com/Abib/joystick-controller-stm32/blob/main/images/uart_terminal_output.png?raw=true)

---

## Conclusion

This project showcases the effective use of a joystick for vehicle control, with real-time feedback displayed on a UART terminal. By leveraging I2C communication, advanced data parsing, and accelerometer integration, this implementation serves as a foundation for complex embedded systems.
