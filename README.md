# ⏰ Digital Clock with Seven-Segment Display on DE1-SoC (FPGA + C)

## 📖 Project Overview  
This project implements a **digital clock** in **24-hour format**, developed on the **Terasic DE1-SoC board**, which combines both a **hardware side (FPGA)** and a **software side (HPS - ARM Cortex-A9)**.

The clock counts and displays **hours, minutes, and seconds** on a **seven-segment display**, and allows **manual adjustment** of the time via a **rotary encoder**. During time adjustment, the selected digit **blinks** to provide clear user feedback.

The system includes custom hardware modules designed in **Platform Designer**, and C code running on the ARM processor within the DE1-SoC.

---

## ⚙️ System Architecture

### 🧠 Software Side (C on ARM HPS)
- Manages the **clock logic**: time counting, encoder control, blinking, and display updates  
- Accesses hardware modules via **memory-mapped registers**  
- Synchronized with a **hardware timer** to ensure precise counting and blinking  
- Communicates with FPGA modules through PIO interfaces and control registers  
- ⚙️ **Runs as bare-metal code** (without any operating system), compiled and uploaded using the **Altera Monitor Program**

### 🧩 Hardware Side (FPGA - Platform Designer)
- Includes:
  - A **timer** module generating a 1 Hz tick  
  - A **PIO module** to read the encoder button  
  - A **display module** controlling the six seven-segment digits  
- All components are accessible from the HPS via **Avalon Memory-Mapped** interfaces

---

## 🔧 System Features

### ⏳ Time Counting
- The **hardware timer** increments once per second using a 100 MHz system clock  
- The C software reads the timer and updates the time (seconds, minutes, hours)  
- Overflows handled in software:  
  - 60 seconds → +1 minute  
  - 60 minutes → +1 hour  
  - 24 hours → reset to 00

### 🎛 Manual Time Adjustment via Encoder
- The **rotary encoder** adjusts one field at a time (hours, minutes, or seconds)  
- The **integrated push button** cycles through modes: seconds → minutes → hours → no adjustment  
- Encoder rotation increases or decreases the selected value  
- Modulo arithmetic used to wrap values correctly (e.g., 59 → 0)

### 💡 Blinking Selected Digit
- During adjustment, the selected digit **blinks** for visual feedback  
- Implemented in software by alternating between the real value and a "blank" (`4'hF`)  
- The display module's **LUT** maps `4'hF` to `7'b1111111` to turn off the digit  
- Blinking frequency is synced with the hardware timer

### 📟 Seven-Segment Display Output
- Time is displayed using six digits:  
  - HEX5–HEX4 → hours  
  - HEX3–HEX2 → minutes  
  - HEX1–HEX0 → seconds  
- Each digit is encoded in BCD and written to a **32-bit display register**  
- Each digit is converted to seven-segment output via **LUTs** in hardware

---

## 🔄 Component Breakdown

### 🔧 Hardware Modules (FPGA)
- **Timer**: counts to 100,000,000 at 100 MHz → 1 tick/second  
- **PIO**: reads encoder push button via GPIO  
- **Display Controller**: manages six digits and includes LUT logic

### 💻 Software in C (on ARM Cortex-A9)
- The `main()` loop continuously:
  - Reads the hardware timer  
  - Updates hours, minutes, seconds  
  - Manages blinking (`blink_state`)  
  - Monitors the encoder and computes delta  
  - Changes mode on button press  
  - Writes current values to the display register  
- ⚠️ The software runs in **bare-metal mode**, without any operating system.  
  It is **compiled and uploaded using the Altera Monitor Program**, which enables direct execution and debugging on the HPS.

---

## 🛠 Technologies Used
- **Terasic DE1-SoC Board**  
- **Intel Quartus Prime + Platform Designer**  
- **C language** running on HPS (bare-metal)  
- **Custom hardware modules**: Timer, PIO, Seven-Segment Display  
- **JTAG UART** for testing/debugging  
- **Memory-mapped I/O** for HPS ↔ FPGA communication

---

## ✅ Project Status
- ✅ Fully working on physical DE1-SoC hardware  
- ✅ Tested with real inputs  
- ✅ Blinking and time control working smoothly  
- ✅ Clean, modular design — ready for future extensions (e.g., alarm, stopwatch)

---

## 🖼️ Hardware Configuration in Platform Designer

The image below shows the hardware setup created in **Intel Quartus Platform Designer**, which defines how all components are connected:

![Platform Designer Hardware Configuration](/img/hardware.jpg)

This configuration sets up:
- The **connection between the ARM HPS and custom FPGA modules** (timer, display, encoder, PIO)  
- The **System_PLL** to generate the required system clocks  
- The **Avalon Memory-Mapped bridges** used by the ARM processor to access FPGA registers  
- The **conduit exports** used to physically map the encoder and display signals to external pins

> 🔧 This structure is essential for the system to work: it ensures that all hardware components can be accessed from the C program running on the ARM processor via memory-mapped I/O. This allows the software to update time, manage user input, and control the display in real-time.

