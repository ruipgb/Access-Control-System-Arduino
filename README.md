 # Arduino Access Control System

This project implements a complete Arduino-based access control system, simulating a real-world automatic gate controller.  
Access is granted only after successful PIN authentication via a keypad, and the system ensures safety through obstacle detection and visual feedback.

---

## 📌 Project Overview

The system is built around an Arduino microcontroller and integrates multiple hardware components to manage secure access control.  
A user must enter a valid PIN code using a 4x3 keypad. If authentication is successful, a servo motor opens the gate.  
Infrared obstacle sensors continuously monitor the gate area to prevent closing while an obstacle is detected.

User feedback is provided through an LCD display and LED indicators, ensuring clear system status visualization at all times.

---

## ⚙️ System Features

- PIN-based authentication using a 4x3 matrix keypad  
- Servo motor control for gate opening and closing  
- Infrared obstacle detection to ensure safety  
- Automatic gate closing when no obstacle is detected  
- LCD display for real-time user feedback  
- Status LEDs for visual indication of system states  
- Periodic status LED blinking at 1 Hz  
- Serial communication for debugging and monitoring  

---

## 🧠 System Logic

1. The system initializes all peripherals (LCD, USART, timers, I/O ports).
2. The user enters a 4-digit PIN via the keypad.
3. The entered PIN is validated against predefined valid PINs.
4. If the PIN is correct:
   - The servo motor opens the gate.
   - The LCD displays a success message.
5. While the gate is open:
   - IR sensors continuously check for obstacles.
   - The gate will not close if an obstacle is detected.
6. When no obstacle is detected:
   - The gate closes automatically.
7. LEDs indicate system status and operating modes.

---

## 🧩 Hardware Components

- Arduino (ATmega-based)
- Servo motor (gate actuator)
- 4x3 matrix keypad
- Infrared obstacle sensors
- 16x2 LCD display
- Status LEDs
- External power supply (5 V)

---

## 🗂️ Project Structure

Access-Control-System-Arduino/
├── arduino/
│ └── main.ino
│ └── lcd.c
│ └── lcd.h
│ └── usart.c
│ └── usart.h
├── script_python/
│ └── usart_diario.py
├── docs/
│ └── report
├── README.md


---

## 🛠️ Development Details

- Language: C / C++ / python / html
- Platform: Arduino / VSCode
- Development Environment: Arduino IDE , VSCode
- Timers and registers are configured directly where applicable
- Modular code structure for readability and maintainability

---

## 📟 User Feedback

- **LCD Display**
  - Prompts for PIN entry
  - Authentication success or failure messages
  - Gate status information

- **LED Indicators**
  - System active indication
  - Visual status feedback at 1 Hz

---

## 🎓 Academic Context

This project was developed as part of an academic engineering context and demonstrates practical knowledge in:
- Embedded systems programming
- Digital I/O manipulation
- Timers and interrupts
- Human-machine interfaces
- Safety mechanisms in control systems

---

## 🚀 Future Improvements

- EEPROM storage for dynamic PIN management
- Buzzer for audio feedback
- Real-time clock (RTC) integration
- Remote monitoring via network or IoT module

---

## 👤 Author

Developed by **Rui Bastos**  
Electrical and Computer Engineering Student
