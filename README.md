 Arduino Access Control System  
Access control system using Arduino, a servo motor for gate control, IR obstacle sensors, a 4×3 keypad for PIN authentication, LCD display for user feedback, and a 1 Hz status LED.

<div align="center">

![Arduino](https://img.shields.io/badge/Arduino-Project-00979D?logo=arduino&logoColor=white)
![C/C++](https://img.shields.io/badge/Language-C/C++-blue)
![Hardware](https://img.shields.io/badge/Hardware-Embedded-orange)
![Status](https://img.shields.io/badge/Status-Active-brightgreen)

</div>

---

## 📌 Overview

This project implements a **complete access management system** using an Arduino microcontroller.  
It simulates a real-world gate controller where access is granted only after PIN validation.  
Once authenticated, the servo motor opens the gate, IR sensors detect obstacles, and the LCD displays meaningful messages.

A blinking LED at **1 Hz** acts as a system heartbeat indicator.

---

## 🛠️ Features

- 🔐 **PIN authentication** using a 4×3 matrix keypad  
- 🚪 **Servo-controlled gate** (open/close logic)  
- 👁️ **IR obstacle detection** for safety  
- 🖥️ **LCD 16×2 display** for system feedback  
- 💡 **1 Hz heartbeat LED** showing system status  
- 🧠 Clean and modular code structure  
- 🛡️ Safety routines to avoid closing the gate while an obstacle is detected  

---

## 🔧 Hardware Used

- Arduino UNO / Nano / similar  
- SG90 or other servo motor  
- 4×3 matrix keypad  
- IR obstacle sensors (HW-201 or equivalent)  
- LCD 16×2 with or without I2C interface  
- LED + resistor (1 Hz heartbeat)  
- Jump wires and breadboard  

---

🔄 Workflow

⌨️ User inputs a PIN using the 4×3 keypad
🔍 System validates the PIN
If the PIN is incorrect → LCD displays “Access Denied”
If the PIN is correct → system proceeds
🚪 Servo motor opens the gate
👁️ IR sensors monitor for obstacles
If an obstacle is detected → gate remains open
LCD displays “Obstacle detected – waiting”
⏳ When no obstacle is detected, the system waits a short delay
🚪 Servo motor closes the gate
💡 Status LED continues blinking at 1 Hz to indicate normal system operation
🔁 System returns to standby mode and waits for the next PIN entry
